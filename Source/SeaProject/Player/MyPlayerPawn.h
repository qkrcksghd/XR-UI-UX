// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "MyPlayerPawn.generated.h"

UCLASS()
class SEAPROJECT_API AMyPlayerPawn : public APawn
{
    GENERATED_BODY()

public:
    AMyPlayerPawn();

protected:
    // 입력 설정을 위한 오버라이드
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

    // 이동 함수
    void MoveForward(float Value);
    void MoveRight(float Value);

    // 회전 함수
    void LookUp(float Value);
    void Turn(float Value);

    // 대시 관련 함수
    void StartDash();
    void StopDash();

private:
    UPROPERTY(VisibleAnywhere)
    class USceneComponent* RootScene;

    UPROPERTY(VisibleAnywhere)
    class UCameraComponent* PlayerCamera;

    UPROPERTY(VisibleAnywhere)
    class UFloatingPawnMovement* MovementComponent;

    // 속도 설정 (에디터에서 수정 가능하도록 EditAnywhere 유지)
    UPROPERTY(EditAnywhere, Category = "Movement")
    float NormalSpeed = 400.0f; // 기본 속도

    UPROPERTY(EditAnywhere, Category = "Movement")
    float DashSpeed = 1200.0f;  // 대시 속도를 3배로 설정

    UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    float MaxBooster = 100.0f;

    UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement", meta = (AllowPrivateAccess = "true"))
    float CurrentBooster = 100.0f;

    UPROPERTY(EditAnywhere, Category = "Movement")
    float BoosterDrainSpeed = 50.0f; // 초당 소모량

    UPROPERTY(EditAnywhere, Category = "Movement")
    float BoosterRegenSpeed = 5.0f; // 초당 회복량

    bool bIsDashing = false; // 현재 대쉬 중인지 체크

    // Tick 함수 추가 (실시간 게이지 계산용)
    virtual void Tick(float DeltaTime) override;
};