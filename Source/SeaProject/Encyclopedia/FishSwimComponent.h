// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "FishSwimComponent.generated.h"

/**
 * 물고기 헤엄 + 벽 회피 컴포넌트.
 *
 * 매 틱 소유 액터를 정면(+X)으로 전진시키고, 전방을 트레이스해 벽/바위가 가까우면 부드럽게
 * 방향을 틀어 피한다. 가끔 랜덤하게 방향을 바꿔(wander) 자연스럽게 배회한다.
 * (벽에 박혀 멈추지 않도록 "부딪히기 전에 미리 트는" 전방 트레이스가 핵심)
 *
 * 주의: 물고기 메시의 정면이 +X 가 아니면 옆/뒤로 헤엄치는 것처럼 보인다 → BP 에서 메시 컴포넌트를
 *       회전시켜 정면을 +X 에 맞추거나, 모델 임포트 방향을 맞춰라.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SEAPROJECT_API UFishSwimComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFishSwimComponent();

protected:
	virtual void BeginPlay() override;

public:
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	/** 전진 속도(cm/s). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSwim")
	float SwimSpeed = 120.0f;

	// ── 도망(스캔당할 때) ─────────────────────────────────────────────
	/** 스캔당해 도망칠 때의 전진 속도(cm/s). 평소 SwimSpeed 보다 크게 잡으면 빠르게 달아난다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSwim|Flee")
	float FleeSpeed = 320.0f;

	/** 도망칠 때 방향 전환 속도(도/초). 위협 반대쪽으로 빠르게 틀도록 평소(TurnSpeedDeg)보다 크게. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSwim|Flee")
	float FleeTurnSpeedDeg = 220.0f;

	/** 도망 시작: 이 위치(위협=플레이어/손전등)에서 멀어지는 쪽으로 빠르게 헤엄친다.
	 *  매 틱 호출하면 위협 위치가 갱신돼 계속 플레이어 반대쪽으로 달아난다. */
	UFUNCTION(BlueprintCallable, Category = "FishSwim|Flee")
	void StartFleeFrom(FVector ThreatLocation);

	/** 도망 해제: 평소 배회(wander)로 돌아간다. */
	UFUNCTION(BlueprintCallable, Category = "FishSwim|Flee")
	void StopFleeing();

	/** 현재 도망 중인지. */
	UFUNCTION(BlueprintPure, Category = "FishSwim|Flee")
	bool IsFleeing() const { return bFleeing; }

	/** 방향 전환 속도(도/초). 작을수록 크게 돌고 느리게 회전. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSwim")
	float TurnSpeedDeg = 90.0f;

	/** 벽 회피 사용 여부. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSwim")
	bool bAvoidWalls = true;

	/** 전방 몇 cm 앞까지 벽을 미리 보는지. 빠른 물고기는 크게. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSwim")
	float WallCheckDistance = 300.0f;

	/** 전방 감지 트레이스의 구체 반경(물고기 몸 굵기 정도). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSwim")
	float WallCheckRadius = 40.0f;

	/** 랜덤 방향 전환(wander) 최소/최대 간격(초). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSwim")
	float WanderIntervalMin = 2.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSwim")
	float WanderIntervalMax = 5.0f;

	/** wander 시 좌우로 틀 수 있는 최대 각도(도). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSwim")
	float WanderYawRange = 60.0f;

	/** 위아래로 헤엄칠 수 있는 최대 기울기(도). 클수록 가파르게 위/아래로 감. 0 이면 수평으로만. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSwim")
	float MaxPitchDeg = 20.0f;

	/** 수심대 가두기를 안 쓸 때, 현재 깊이에서 위/아래로 돌아다닐 범위(cm). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSwim")
	float VerticalRange = 400.0f;

	/** 메시 정면이 +X 가 아닐 때 보정(Pitch/Yaw/Roll, 도). 물고기가 옆/뒤/위로 가는 것처럼 보이면
	 *  여기서 돌려 맞춘다. (이동은 액터 +X 기준 그대로, 메시 비주얼만 회전) — 종마다 다르면 스포너에서 종별 지정. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSwim")
	FRotator MeshRotationOffset = FRotator::ZeroRotator;

	/** 디버그: 이동 방향(정면)을 화살표로 표시. 메시 코가 이 화살표를 향하게 MeshYawOffset 을 맞추면 됨. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSwim")
	bool bDrawForwardArrow = false;

	// ── 수심대 가두기 ─────────────────────────────────────────────────
	/** 이 물고기를 특정 Z 범위(수심대) 안에 머물게 한다(스포너가 종 수심으로 설정). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSwim|Depth")
	bool bClampDepth = false;

	/** 가둘 Z 하한(가장 깊은 곳). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSwim|Depth")
	float MinZ = -100000.0f;

	/** 가둘 Z 상한(가장 얕은 곳). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSwim|Depth")
	float MaxZ = 100000.0f;

private:
	// 현재 향하려는 목표 회전(여기로 부드럽게 보간).
	FRotator TargetRot = FRotator::ZeroRotator;

	// 향하는 목표 수심(Z). wander 때마다 새로 고른다 → 위/아래로 헤엄침.
	float WanderTargetZ = 0.0f;

	// 다음 wander 까지 남은 시간.
	float WanderTimer = 0.0f;

	// 도망 상태 + 위협(멀어질 기준) 위치.
	bool bFleeing = false;
	FVector FleeFromLocation = FVector::ZeroVector;

	// 새 랜덤 방향을 고른다.
	void PickWander();
};
