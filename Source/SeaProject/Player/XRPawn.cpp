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

	LeftGrabSphere = CreateDefaultSubobject<USphereComponent>(TEXT("LeftGrabSphere"));
	LeftGrabSphere->SetupAttachment(LeftHandController);
	LeftGrabSphere->SetSphereRadius(10.0f); // 그랩 반경 설정
	LeftGrabSphere->SetCollisionProfileName(TEXT("Trigger")); // 물리 충돌 대신 트리거로 사용

	// 3. 모션 컨트롤러 설정 (오른손)
	RightHandController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightHandController"));
	RightHandController->SetupAttachment(VRRoot);
	RightHandController->SetTrackingMotionSource(FXRMotionControllerBase::RightHandSourceId);

	RightGrabSphere = CreateDefaultSubobject<USphereComponent>(TEXT("RightGrabSphere"));
	RightGrabSphere->SetupAttachment(RightHandController);
	RightGrabSphere->SetSphereRadius(10.0f);
	RightGrabSphere->SetCollisionProfileName(TEXT("Trigger"));
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

		// 그랩 (왼손) - 버튼 누름(Started), 뗌(Completed)
		if (GrabLeftAction)
		{
			EnhancedInputComp->BindAction(GrabLeftAction, ETriggerEvent::Started, this, &AXRPawn::Input_GrabLeft_Started);
			EnhancedInputComp->BindAction(GrabLeftAction, ETriggerEvent::Completed, this, &AXRPawn::Input_GrabLeft_Completed);
		}

		// 그랩 (오른손)
		if (GrabRightAction)
		{
			EnhancedInputComp->BindAction(GrabRightAction, ETriggerEvent::Started, this, &AXRPawn::Input_GrabRight_Started);
			EnhancedInputComp->BindAction(GrabRightAction, ETriggerEvent::Completed, this, &AXRPawn::Input_GrabRight_Completed);
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

// 왼쪽 그랩 처리
void AXRPawn::Input_GrabLeft_Started() { AttemptGrab(LeftGrabSphere, LeftHandController); }
void AXRPawn::Input_GrabLeft_Completed() { ReleaseGrab(LeftHandController); }

// 오른쪽 그랩 처리
void AXRPawn::Input_GrabRight_Started() { AttemptGrab(RightGrabSphere, RightHandController); }
void AXRPawn::Input_GrabRight_Completed() { ReleaseGrab(RightHandController); }


// ── 그랩 로직 구현 (간단한 버전) ────────────────────────────────────────────

void AXRPawn::AttemptGrab(USphereComponent* GrabSphere, UMotionControllerComponent* TargetController)
{
	// 이미 잡고 있다면 리턴
	if ((TargetController == LeftHandController && HeldActorLeft) || (TargetController == RightHandController && HeldActorRight))
	{
		return;
	}

	// 1. GrabSphere 영역 내에 있는 액터 찾기 (Sphere Overlap Actor)
	TArray<AActor*> OverlappingActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody)); // 물리 액터만 감지

	// Kismet 라이브러리를 사용하여 간단하게 Overlap 체크
	UKismetSystemLibrary::SphereOverlapActors(
		GrabSphere,
		GrabSphere->GetComponentLocation(),
		GrabSphere->GetScaledSphereRadius(),
		ObjectTypes,
		AActor::StaticClass(), // 모든 액터 클래스
		TArray<AActor*>(), // 무시할 액터 없음
		OverlappingActors
	);

	// 2. 가장 가까운 액터 찾기 및 Attach
	AActor* ClosestActor = nullptr;
	float MinDistance = FLT_MAX;

	for (AActor* Actor : OverlappingActors)
	{
		// 자기 자신(Pawn)은 무시
		if (Actor == this) continue;

		// 액터의 스태틱 메쉬가 있는지 확인하고 물리 시뮬레이션 중인지 체크 (잡으려면 물리가 켜져 있어야 함)
		UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(Actor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (MeshComp && MeshComp->IsSimulatingPhysics())
		{
			float Distance = FVector::Dist(GrabSphere->GetComponentLocation(), Actor->GetActorLocation());
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				ClosestActor = Actor;
			}
		}
	}

	if (ClosestActor)
	{
		// 3. 액터 Attach (잡기)
		UE_LOG(LogTemp, Warning, TEXT("Grabbed Actor: %s"), *ClosestActor->GetName());

		// 스태틱 메쉬의 물리를 끄고 컨트롤러에 Attach
		UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(ClosestActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (MeshComp)
		{
			MeshComp->SetSimulatePhysics(false); // 잡았을 때는 물리를 꺼야 함
		}

		// 컨트롤러 컴포넌트에 액터를 Attach (위치, 회전 유지)
		ClosestActor->AttachToComponent(TargetController, FAttachmentTransformRules::KeepWorldTransform);

		// 잡고 있는 액터 변수에 저장
		if (TargetController == LeftHandController) { HeldActorLeft = ClosestActor; }
		else { HeldActorRight = ClosestActor; }
	}
}

void AXRPawn::ReleaseGrab(UMotionControllerComponent* TargetController)
{
	AActor* ActorToRelease = (TargetController == LeftHandController) ? HeldActorLeft : HeldActorRight;

	if (ActorToRelease)
	{
		UE_LOG(LogTemp, Warning, TEXT("Released Actor: %s"), *ActorToRelease->GetName());

		// 1. Detach (떼어내기)
		ActorToRelease->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// 2. 물리 다시 켜기
		UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(ActorToRelease->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (MeshComp)
		{
			MeshComp->SetSimulatePhysics(true);
		}

		// 3. 변수 초기화
		if (TargetController == LeftHandController) { HeldActorLeft = nullptr; }
		else { HeldActorRight = nullptr; }
	}
}