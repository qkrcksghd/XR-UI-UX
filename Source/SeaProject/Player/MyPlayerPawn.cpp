// Fill out your copyright notice in the Description page of Project Settings.

#include "MyPlayerPawn.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/FloatingPawnMovement.h"

AMyPlayerPawn::AMyPlayerPawn() //생성자
{
	PrimaryActorTick.bCanEverTick = true; // 매 프레임마다 Tick() 함수를 호출하도록 설정 (필요에 따라 true로 설정)

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));    // 루트 컴포넌트 생성
	RootComponent = RootScene; //  루트 컴포넌트를 RootScene으로 설정

	PlayerCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("PlayerCamera")); // 카메라 컴포넌트 생성
	PlayerCamera->SetupAttachment(RootComponent);// 카메라를 루트 컴포넌트에 부착

    // 1인칭 필수 설정: 카메라가 플레이어 컨트롤러의 회전값을 그대로 따르게 합니다.
	PlayerCamera->bUsePawnControlRotation = true;// 카메라가 폰의 회전값을 따르도록 설정

    MovementComponent = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComponent"));

    
	bUseControllerRotationYaw = true; //마우스로 좌우로 돌릴 때 폰 몸체도 같이 회전하게 하려면 true (일반적인 PC FPS 방식)
	bUseControllerRotationPitch = true;// 마우스로 좌우로 돌릴 때 폰 몸체도 같이 회전하게 하려면 true (일반적인 PC FPS 방식)
}

void AMyPlayerPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent) // 입력 바인딩 설정
{
	Super::SetupPlayerInputComponent(PlayerInputComponent); // 부모 클래스의 입력 설정도 호출

    // 이동 바인딩
    PlayerInputComponent->BindAxis("MoveForward", this, &AMyPlayerPawn::MoveForward); //앞뒤
    PlayerInputComponent->BindAxis("MoveRight", this, &AMyPlayerPawn::MoveRight); //좌우

    // 회전 바인딩 추가
	PlayerInputComponent->BindAxis("LookUp", this, &AMyPlayerPawn::LookUp);//마우스 Y축 (위아래)
	PlayerInputComponent->BindAxis("Turn", this, &AMyPlayerPawn::Turn);//마우스 X축 (좌우)
}

void AMyPlayerPawn::MoveForward(float Value) {
	if (Value != 0.f) AddMovementInput(GetActorForwardVector(), Value);// 폰의 앞 방향으로 입력값만큼 이동하도록 함
}

void AMyPlayerPawn::MoveRight(float Value) {
	if (Value != 0.f) AddMovementInput(GetActorRightVector(), Value);// 폰의 오른쪽 방향으로 입력값만큼 이동하도록 함
}

void AMyPlayerPawn::LookUp(float Value) {
	AddControllerPitchInput(Value); // 마우스 Y축 입력값을 컨트롤러의 Pitch에 더하여 위아래 회전 구현
}

void AMyPlayerPawn::Turn(float Value) {
	AddControllerYawInput(Value); // 마우스 X축 입력값을 컨트롤러의 Yaw에 더하여 좌우 회전 구현
}
