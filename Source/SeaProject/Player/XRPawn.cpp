// Fill out your copyright notice in the Description page of Project Settings.

#include "XRPawn.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"
#include "Components/SphereComponent.h"
#include "Kismet/KismetSystemLibrary.h" // Overlap 체크용
#include "XRMotionControllerBase.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

AXRPawn::AXRPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	
	MovementComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComp"));
	// 1. 기본 계층 구조 설정 (VRRoot -> Camera)
	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	SetRootComponent(VRRoot);
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VRRoot);
	Camera->bUsePawnControlRotation = false;
	// HMD 위치에 따라 카메라가 자동으로 움직이도록 설정
	Camera->bUsePawnControlRotation = false;

	// 2. 모션 컨트롤러 설정 (왼손)
	LeftHandController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftHandController"));
	LeftHandController->SetupAttachment(VRRoot);
	LeftHandController->SetTrackingMotionSource(FXRMotionControllerBase::LeftHandSourceId);

	// 3. 모션 컨트롤러 설정 (오른손)
	RightHandController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightHandController"));
	RightHandController->SetupAttachment(VRRoot);
	RightHandController->SetTrackingMotionSource(FXRMotionControllerBase::RightHandSourceId);
}

void AXRPawn::BeginPlay()
{
	Super::BeginPlay();
}

void AXRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

// ── 입력 바인딩 ───────────────────────────────────────────────────────────
void AXRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 1. 빙의가 완료된 확실한 타이밍에 매핑 컨텍스트(IMC_VR) 장착!
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (VRMappingContext)
			{
				Subsystem->AddMappingContext(VRMappingContext, 0);
			}
		}
	}
	// EnhancedInputComponent로 캐스팅
	if (UEnhancedInputComponent* EnhancedInputComp = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 이동 (조이스틱)
		if (MoveAction)
		{
			EnhancedInputComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AXRPawn::Input_Move);
		}
	}
}

// ── 입력 처리 함수 구현 ────────────────────────────────────────────────────

void AXRPawn::Input_Move(const FInputActionValue& Value)
{
	// 조이스틱의 Vector2D 값 가져오기
	FVector2D MoveVector = Value.Get<FVector2D>();

	if (Controller && (MoveVector.X != 0.0f || MoveVector.Y != 0.0f))
	{
		// 카메라가 바라보는 방향 기준 이동
		FRotator CameraRotation = Camera->GetComponentRotation();
		FRotator YawRotation(0.0f, CameraRotation.Yaw, 0.0f);

		// 전진/후진 (Y축)
		FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection, MoveVector.Y * MoveSpeed * GetWorld()->GetDeltaSeconds());

		// 좌우 (X축)
		FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection, MoveVector.X * MoveSpeed * GetWorld()->GetDeltaSeconds());
	}
}