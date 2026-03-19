// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h" // Enhanced Input 필수 헤더
#include "XRPawn.generated.h"

// 컴포넌트 전방 선언 (빌드 속도 향상)
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class USphereComponent;
class USkeletalMeshComponent;
class UInputMappingContext;
class UInputAction;
class UFloatingPawnMovement;

UCLASS()
class SEAPROJECT_API AXRPawn : public APawn
{
	GENERATED_BODY()

public:
	AXRPawn();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// 입력 컴포넌트 설정 오버라이드
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ── 계층 구조 컴포넌트 ───────────────────────────────────────────
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Components")
	USceneComponent* VRRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Components")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Components")
	UMotionControllerComponent* LeftHandController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Components")
	UMotionControllerComponent* RightHandController;

	// 그랩 범위를 시각화하거나 로직에 사용할 충돌 구체
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Components")
	USphereComponent* LeftGrabSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Components")
	USphereComponent* RightGrabSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Components")
	class UFloatingPawnMovement* MovementComp;
	// ── 향상된 입력 (Enhanced Input) ────────────────────────────────
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XR | Input")
	UInputMappingContext* VRMappingContext;

	// 이동 입력 (Vector2D: 조이스틱 X, Y)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XR | Input")
	UInputAction* MoveAction;

	// 그랩 입력 (Digital: 버튼 누름/뗌)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XR | Input")
	UInputAction* GrabLeftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XR | Input")
	UInputAction* GrabRightAction;

	// ── 이동 속도 설정 ────────────────────────────────────────────────
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Movement")
	float MoveSpeed = 300.0f;

private:
	// ── 입력 처리 함수 (Binding) ──────────────────────────────────
	void Input_Move(const FInputActionValue& Value);

	// 왼쪽 그랩
	void Input_GrabLeft_Started();
	void Input_GrabLeft_Completed();

	// 오른쪽 그랩
	void Input_GrabRight_Started();
	void Input_GrabRight_Completed();

	// ── 그랩 로직 함수 ──────────────────────────────────────────────
	void AttemptGrab(USphereComponent* GrabSphere, UMotionControllerComponent* TargetController);
	void ReleaseGrab(UMotionControllerComponent* TargetController);

	// 현재 잡고 있는 액터를 저장할 변수 (간단한 구현용)
	UPROPERTY()
	AActor* HeldActorLeft;

	UPROPERTY()
	AActor* HeldActorRight;
};