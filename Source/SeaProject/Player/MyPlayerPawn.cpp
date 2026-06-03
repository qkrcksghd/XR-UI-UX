// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

AMyPlayerPawn::AMyPlayerPawn()
{
    PrimaryActorTick.bCanEverTick = true;

    // 1. 루트 컴포넌트 설정
    RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
    RootComponent = RootScene;

    // 2. 카메라 설정
    PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera"));
    PlayerCamera->SetupAttachment(RootComponent);
    PlayerCamera->bUsePawnControlRotation = true;

    // 3. 무브먼트 컴포넌트 설정
    MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));

    // 초기 물리 설정 (가속도/감속도를 높여야 부스터 체감이 좋습니다)
    MovementComponent->MaxSpeed = NormalSpeed;
    MovementComponent->Acceleration = 6000.f;
    MovementComponent->Deceleration = 6000.f;

    // 폰 회전 설정
    bUseControllerRotationYaw = true;
    bUseControllerRotationPitch = true;
}

void AMyPlayerPawn::Tick(float DeltaTime)
{
    Super::Tick(DeltaTime);

    // 부스터(대시) 게이지 처리 로직
    if (bIsDashing && CurrentBooster > 0.0f)
    {
        // 대시 중: 게이지 소모
        CurrentBooster -= BoosterDrainSpeed * DeltaTime;

        if (CurrentBooster <= 0.0f)
        {
            CurrentBooster = 0.0f;
            StopDash(); // 게이지 다 쓰면 강제 중지
        }
    }
    else if (!bIsDashing && CurrentBooster < MaxBooster)
    {
        // 대시 안 함: 게이지 회복
        CurrentBooster += BoosterRegenSpeed * DeltaTime;
        CurrentBooster = FMath::Clamp(CurrentBooster, 0.0f, MaxBooster);
    }
}

void AMyPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
    Super::SetupPlayerInputComponent(PlayerInputComponent);

    // 이동 바인딩
    PlayerInputComponent->BindAxis("MoveForward", this, &AMyPlayerPawn::MoveForward);
    PlayerInputComponent->BindAxis("MoveRight", this, &AMyPlayerPawn::MoveRight);

    // 회전 바인딩
    PlayerInputComponent->BindAxis("LookUp", this, &AMyPlayerPawn::LookUp);
    PlayerInputComponent->BindAxis("Turn", this, &AMyPlayerPawn::Turn);

    // 대시 액션 바인딩
    PlayerInputComponent->BindAction("Dash", IE_Pressed, this, &AMyPlayerPawn::StartDash);
    PlayerInputComponent->BindAction("Dash", IE_Released, this, &AMyPlayerPawn::StopDash);
}

void AMyPlayerPawn::StartDash()
{
    // 게이지가 10% 이상일 때만 대시 시작 가능
    if (CurrentBooster > 10.0f)
    {
        bIsDashing = true;
        if (MovementComponent)
        {
            MovementComponent->MaxSpeed = DashSpeed;
            UE_LOG(LogTemp, Warning, TEXT("Booster Active! Speed: %f"), DashSpeed);
        }
    }
}

void AMyPlayerPawn::StopDash()
{
    bIsDashing = false;
    if (MovementComponent)
    {
        MovementComponent->MaxSpeed = NormalSpeed;
        UE_LOG(LogTemp, Warning, TEXT("Booster Off! Speed: %f"), NormalSpeed);
    }
}

void AMyPlayerPawn::MoveForward(float Value)
{
    if (Value != 0.f) AddMovementInput(GetActorForwardVector(), Value);
}

void AMyPlayerPawn::MoveRight(float Value)
{
    if (Value != 0.f) AddMovementInput(GetActorRightVector(), Value);
}

void AMyPlayerPawn::LookUp(float Value)
{
    AddControllerPitchInput(Value);
}

void AMyPlayerPawn::Turn(float Value)
{
    AddControllerYawInput(Value);
}