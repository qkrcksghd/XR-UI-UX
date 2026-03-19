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
    virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

private:
    UPROPERTY(VisibleAnywhere)
    class USceneComponent* RootScene;

    UPROPERTY(VisibleAnywhere)
    class UCameraComponent* PlayerCamera;

    UPROPERTY(VisibleAnywhere)
    class UFloatingPawnMovement* MovementComponent;

    // 이동 함수
    void MoveForward(float Value);
    void MoveRight(float Value);

    // 회전 함수 추가
    void LookUp(float Value); // 마우스 Y축 (위아래)
    void Turn(float Value);   // 마우스 X축 (좌우)
};