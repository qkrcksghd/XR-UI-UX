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
class USpotLightComponent;
class UFlashlightScanComponent;

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

	// ── 손전등 스캔 ───────────────────────────────────────────────────
	// 오른손 컨트롤러에 붙는 손전등(빛 연출 + 트레이스 방향 기준)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Scan")
	USpotLightComponent* Flashlight;

	// 손전등 빛으로 물고기를 스캔하는 컴포넌트 (Sonar 스캐너와 별개)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Scan")
	UFlashlightScanComponent* ScanComponent;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Components")
	class UFloatingPawnMovement* MovementComp;
	// ── 향상된 입력 (Enhanced Input) ────────────────────────────────
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XR | Input")
	UInputMappingContext* VRMappingContext;

	// 이동 입력 (Vector2D: 조이스틱 X, Y)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XR | Input")
	UInputAction* MoveAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XR | Input")
	UInputAction* GrabRightAction;

	// 대시 입력. IMC_VR 에서 이 액션에 원하는 버튼들(트리거/그립/A/B 등)을
	// 전부 매핑하면 "아무 버튼이나 누르면 대시"가 된다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XR | Input")
	UInputAction* DashAction;

	// ── 이동 속도 설정 ────────────────────────────────────────────────
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Movement")
	float MoveSpeed = 300.0f;

	// 대시(부스트) 중 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Movement")
	float DashSpeed = 900.0f;

	// 현재 대시 중인지 (읽기 전용)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Movement")
	bool bIsDashing = false;

private:
	// ── 입력 처리 함수 (Binding) ──────────────────────────────────
	void Input_Move(const FInputActionValue& Value);

	// 대시 시작/종료 (버튼 누름/뗌)
	void StartDash();
	void StopDash();
};