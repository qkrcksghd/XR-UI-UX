// Fill out your copyright notice in the Description page of Project Settings.

#include "Menu/MenuPawn.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"
#include "XRMotionControllerBase.h"
#include "Components/WidgetComponent.h"
#include "Components/WidgetInteractionComponent.h"
#include "Components/StaticMeshComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/PlayerController.h"

AMenuPawn::AMenuPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	SetRootComponent(VRRoot);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VRRoot);
	Camera->bUsePawnControlRotation = false; // HMD가 카메라를 직접 구동

	LeftHandController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftHandController"));
	LeftHandController->SetupAttachment(VRRoot);
	LeftHandController->SetTrackingMotionSource(FXRMotionControllerBase::LeftHandSourceId);

	RightHandController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightHandController"));
	RightHandController->SetupAttachment(VRRoot);
	RightHandController->SetTrackingMotionSource(FXRMotionControllerBase::RightHandSourceId);

	// 2D 메뉴판 — World 공간 위젯. 위치/회전은 Tick에서 카메라 정면으로 몬다.
	MenuPanel = CreateDefaultSubobject<UWidgetComponent>(TEXT("MenuPanel"));
	MenuPanel->SetupAttachment(VRRoot);
	MenuPanel->SetWidgetSpace(EWidgetSpace::World);
	MenuPanel->SetDrawSize(FVector2D(1000.0f, 600.0f)); // WBP_MainMenu 디자인 해상도에 맞게 BP에서 조정 가능
	MenuPanel->SetTwoSided(true);
	// 레이저(WidgetInteraction, TraceChannel=Visibility)가 패널을 맞히도록 콜리전 설정
	MenuPanel->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	MenuPanel->SetCollisionResponseToAllChannels(ECR_Ignore);
	MenuPanel->SetCollisionResponseToChannel(ECC_Visibility, ECR_Block);

	// 오른손 레이저(위젯 상호작용)
	WidgetInteraction = CreateDefaultSubobject<UWidgetInteractionComponent>(TEXT("WidgetInteraction"));
	WidgetInteraction->SetupAttachment(RightHandController);
	WidgetInteraction->InteractionDistance = PointerDistance;
	WidgetInteraction->InteractionSource = EWidgetInteractionSource::World;
	WidgetInteraction->TraceChannel = ECC_Visibility;
	WidgetInteraction->bShowDebug = false;

	// 레이저 빔 — 엔진 기본 실린더(Z축 100uu)를 -90 피치로 눕혀 +X 정면을 향하게.
	// WidgetInteraction 에 붙여 조준 오프셋까지 그대로 따라가게 한다.
	LaserBeam = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserBeam"));
	LaserBeam->SetupAttachment(WidgetInteraction);
	LaserBeam->SetRelativeRotation(FRotator(-90.0f, 0.0f, 0.0f)); // 로컬 +Z → 부모 +X
	LaserBeam->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LaserBeam->SetCastShadow(false);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> CylinderMesh(TEXT("/Engine/BasicShapes/Cylinder.Cylinder"));
	if (CylinderMesh.Succeeded())
	{
		LaserBeam->SetStaticMesh(CylinderMesh.Object);
	}

	// 끝점 도트 — 작은 구. WidgetInteraction 정면(+X)으로 hit 거리만큼 떨어뜨려 둔다.
	LaserDot = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("LaserDot"));
	LaserDot->SetupAttachment(WidgetInteraction);
	LaserDot->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	LaserDot->SetCastShadow(false);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> SphereMesh(TEXT("/Engine/BasicShapes/Sphere.Sphere"));
	if (SphereMesh.Succeeded())
	{
		LaserDot->SetStaticMesh(SphereMesh.Object);
	}
}

void AMenuPawn::BeginPlay()
{
	Super::BeginPlay();

	if (WidgetInteraction)
	{
		WidgetInteraction->InteractionDistance = PointerDistance;
		WidgetInteraction->SetRelativeRotation(PointerAimOffset);
	}
	if (LaserBeam)
	{
		LaserBeam->SetVisibility(bShowLaser);
	}
	if (LaserDot)
	{
		LaserDot->SetVisibility(false); // 맞힌 게 있을 때만 표시
	}
}

void AMenuPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	UpdatePanelTransform(DeltaTime);
	UpdateLaser();
}

void AMenuPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (MenuMappingContext)
			{
				Subsystem->AddMappingContext(MenuMappingContext, 0);
			}
		}
	}

	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 트리거 = 좌클릭. Started=당김(누름), Completed/Canceled=뗌
		if (ClickAction)
		{
			EnhancedInputComp->BindAction(ClickAction, ETriggerEvent::Started, this, &AMenuPawn::Click_Pressed);
			EnhancedInputComp->BindAction(ClickAction, ETriggerEvent::Completed, this, &AMenuPawn::Click_Released);
			EnhancedInputComp->BindAction(ClickAction, ETriggerEvent::Canceled, this, &AMenuPawn::Click_Released);
		}
	}
}

// 패널을 카메라 정면 PanelDistance 앞으로 부드럽게 이동/회전.
void AMenuPawn::UpdatePanelTransform(float DeltaTime)
{
	if (!Camera || !MenuPanel)
	{
		return;
	}

	const FVector CamLoc = Camera->GetComponentLocation();
	const FVector CamFwd = Camera->GetForwardVector();
	const FVector TargetLoc = CamLoc + CamFwd * PanelDistance + FVector(0.0f, 0.0f, PanelVerticalOffset);

	// 패널이 카메라를 바라보게: 위젯의 보이는 면(+X)이 카메라를 향하도록.
	FRotator TargetRot = (CamLoc - TargetLoc).Rotation();
	if (bFlipPanelFacing)
	{
		TargetRot.Yaw += 180.0f;
	}

	if (!bPanelInitialized)
	{
		// 첫 틱: 곧바로 목표로 스냅(원점에서 날아오는 것 방지)
		MenuPanel->SetWorldLocationAndRotation(TargetLoc, TargetRot);
		bPanelInitialized = true;
		return;
	}

	const FVector NewLoc = FMath::VInterpTo(MenuPanel->GetComponentLocation(), TargetLoc, DeltaTime, FollowSpeed);
	const FRotator NewRot = FMath::RInterpTo(MenuPanel->GetComponentRotation(), TargetRot, DeltaTime, FollowSpeed);
	MenuPanel->SetWorldLocationAndRotation(NewLoc, NewRot);
}

// 레이저 빔 길이 = hit 지점까지, 끝점 도트는 hit 지점에.
void AMenuPawn::UpdateLaser()
{
	if (!WidgetInteraction || !LaserBeam || !LaserDot)
	{
		return;
	}

	if (!bShowLaser)
	{
		LaserBeam->SetVisibility(false);
		LaserDot->SetVisibility(false);
		return;
	}

	const FHitResult& Hit = WidgetInteraction->GetLastHitResult();
	const float Length = Hit.bBlockingHit ? Hit.Distance : PointerDistance;

	// 빔: -90 피치로 누운 실린더 → 로컬 +Z가 부모(+X 정면). 길이=Length, 두께=0.5cm
	LaserBeam->SetVisibility(true);
	LaserBeam->SetRelativeScale3D(FVector(0.01f, 0.01f, Length / 100.0f));
	LaserBeam->SetRelativeLocation(FVector(Length * 0.5f, 0.0f, 0.0f));

	// 도트: 맞힌 지점(+X로 Length)에 작은 구. 못 맞히면 숨김.
	if (Hit.bBlockingHit)
	{
		LaserDot->SetVisibility(true);
		LaserDot->SetRelativeScale3D(FVector(0.04f)); // 지름 약 4cm
		LaserDot->SetRelativeLocation(FVector(Length, 0.0f, 0.0f));
	}
	else
	{
		LaserDot->SetVisibility(false);
	}
}

void AMenuPawn::Click_Pressed()
{
	if (WidgetInteraction)
	{
		WidgetInteraction->PressPointerKey(EKeys::LeftMouseButton);
	}
}

void AMenuPawn::Click_Released()
{
	if (WidgetInteraction)
	{
		WidgetInteraction->ReleasePointerKey(EKeys::LeftMouseButton);
	}
}
