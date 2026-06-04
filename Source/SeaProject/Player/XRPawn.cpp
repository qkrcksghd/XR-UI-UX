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
#include "Components/SpotLightComponent.h"
#include "Scan/FlashlightScanComponent.h"

AXRPawn::AXRPawn()
{
	PrimaryActorTick.bCanEverTick = true;
	
	MovementComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComp"));
	MovementComp->MaxSpeed = MoveSpeed; // 실제 이동 속도는 MaxSpeed가 결정 (아래 Input_Move 참고)
	// 1. 기본 계층 구조 설정 (VRRoot -> Camera)
	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	SetRootComponent(VRRoot);
	
	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VRRoot);
	// HMD(헤드셋) 위치/회전이 카메라를 직접 구동하도록 컨트롤러 회전 사용 안 함
	Camera->bUsePawnControlRotation = false;

	// 2. 모션 컨트롤러 설정 (왼손)
	LeftHandController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftHandController"));
	LeftHandController->SetupAttachment(VRRoot);
	LeftHandController->SetTrackingMotionSource(FXRMotionControllerBase::LeftHandSourceId);

	// 3. 모션 컨트롤러 설정 (오른손)
	RightHandController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightHandController"));
	RightHandController->SetupAttachment(VRRoot);
	RightHandController->SetTrackingMotionSource(FXRMotionControllerBase::RightHandSourceId);

	// 4. 손전등 (오른손 컨트롤러에 부착) — 빛 연출 + 스캔 트레이스 방향 기준
	Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
	Flashlight->SetupAttachment(RightHandController);
	Flashlight->SetIntensity(5000.0f);
	Flashlight->SetAttenuationRadius(2000.0f);
	Flashlight->SetInnerConeAngle(12.0f);
	Flashlight->SetOuterConeAngle(25.0f);

	// 5. 손전등 스캔 컴포넌트 (Sonar 스캐너와 독립)
	ScanComponent = CreateDefaultSubobject<UFlashlightScanComponent>(TEXT("ScanComponent"));
}

void AXRPawn::BeginPlay()
{
	Super::BeginPlay();

	// 스캔 컴포넌트가 손전등 방향을 기준으로 트레이스하도록 연결
	if (ScanComponent && Flashlight)
	{
		ScanComponent->SetTraceSource(Flashlight);
	}
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

		// 대시 (버튼 누르는 동안 부스트) — Started=누름, Completed/Canceled=뗌
		if (DashAction)
		{
			EnhancedInputComp->BindAction(DashAction, ETriggerEvent::Started, this, &AXRPawn::StartDash);
			EnhancedInputComp->BindAction(DashAction, ETriggerEvent::Completed, this, &AXRPawn::StopDash);
			EnhancedInputComp->BindAction(DashAction, ETriggerEvent::Canceled, this, &AXRPawn::StopDash);
		}
	}
}

// ── 대시 ────────────────────────────────────────────────────────────────────
void AXRPawn::StartDash()
{
	bIsDashing = true;
	if (MovementComp)
	{
		MovementComp->MaxSpeed = DashSpeed;
	}
}

void AXRPawn::StopDash()
{
	bIsDashing = false;
	if (MovementComp)
	{
		MovementComp->MaxSpeed = MoveSpeed;
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

		// 전진/후진 (Y축) — 스케일은 조이스틱 입력값(-1~1)만 전달.
		// 실제 속도/프레임보정은 FloatingPawnMovement(MaxSpeed)가 처리하므로
		// 여기서 MoveSpeed나 DeltaSeconds를 곱하면 안 됨(이중 적용 + 입력이 1로 포화됨).
		FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);
		AddMovementInput(ForwardDirection, MoveVector.Y);

		// 좌우 (X축)
		FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);
		AddMovementInput(RightDirection, MoveVector.X);
	}
}