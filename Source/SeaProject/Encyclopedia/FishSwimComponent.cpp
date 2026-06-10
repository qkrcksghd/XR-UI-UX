// Fill out your copyright notice in the Description page of Project Settings.

#include "FishSwimComponent.h"
#include "GameFramework/Actor.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"
#include "Components/SkeletalMeshComponent.h"
#include "DrawDebugHelpers.h"

UFishSwimComponent::UFishSwimComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFishSwimComponent::BeginPlay()
{
	Super::BeginPlay();

	if (AActor* Owner = GetOwner())
	{
		TargetRot = Owner->GetActorRotation();

		// 메시 정면 보정: 메시 컴포넌트만 회전시켜 비주얼 정면을 액터 +X 에 맞춘다.
		// (generic 물고기는 메쉬가 씬 루트의 자식이라, 여기서 메쉬만 돌려도 액터 회전에 안 덮인다)
		if (!MeshRotationOffset.IsNearlyZero())
		{
			if (USkeletalMeshComponent* SkComp = Owner->FindComponentByClass<USkeletalMeshComponent>())
			{
				SkComp->SetRelativeRotation(MeshRotationOffset);
			}
		}
	}
	PickWander();
}

void UFishSwimComponent::PickWander()
{
	// 좌우 랜덤 전환
	TargetRot.Yaw += FMath::FRandRange(-WanderYawRange, WanderYawRange);
	WanderTimer = FMath::FRandRange(WanderIntervalMin, WanderIntervalMax);

	// 새 목표 수심(Z) 고르기 → 그쪽으로 위/아래로 헤엄친다.
	if (const AActor* Owner = GetOwner())
	{
		const float CurZ = Owner->GetActorLocation().Z;
		WanderTargetZ = bClampDepth
			? FMath::FRandRange(MinZ, MaxZ)                                  // 수심대 안 아무 깊이
			: CurZ + FMath::FRandRange(-VerticalRange, VerticalRange);       // 현재서 위/아래로
	}
}

void UFishSwimComponent::StartFleeFrom(FVector ThreatLocation)
{
	bFleeing = true;
	FleeFromLocation = ThreatLocation;
}

void UFishSwimComponent::StopFleeing()
{
	bFleeing = false;
}

void UFishSwimComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!Owner || !World)
	{
		return;
	}

	const FVector Loc = Owner->GetActorLocation();

	// 도망 중이면 평소보다 빠른 속도/회전을 쓴다.
	const float CurSwimSpeed = bFleeing ? FleeSpeed : SwimSpeed;
	const float CurTurnSpeed = bFleeing ? FleeTurnSpeedDeg : TurnSpeedDeg;

	// 1) 방향 결정
	if (bFleeing)
	{
		// 위협(플레이어)에서 멀어지는 수평 방향으로 즉시 목표를 잡는다.
		FVector Away = Loc - FleeFromLocation;
		Away.Z = 0.0f;
		if (!Away.IsNearlyZero())
		{
			TargetRot.Yaw = Away.Rotation().Yaw;
		}
		// 위/아래로도 위협 반대쪽으로 향한다(수심대 가두기가 켜져 있으면 그 범위 안으로).
		float VertTarget = Loc.Z + ((Loc.Z >= FleeFromLocation.Z) ? VerticalRange : -VerticalRange);
		WanderTargetZ = bClampDepth ? FMath::Clamp(VertTarget, MinZ, MaxZ) : VertTarget;
	}
	else
	{
		// 주기적 랜덤 방향 전환(평소 배회)
		WanderTimer -= DeltaTime;
		if (WanderTimer <= 0.0f)
		{
			PickWander();
		}
	}

	// 2) 전방에 벽/바위가 가까우면 미리 방향 틀기 (박기 전에)
	if (bAvoidWalls)
	{
		const FVector Fwd = Owner->GetActorForwardVector();
		const FVector End = Loc + Fwd * WallCheckDistance;

		FHitResult Hit;
		FCollisionQueryParams Params(SCENE_QUERY_STAT(FishSwimWallCheck), false, Owner);
		const bool bBlocked = World->SweepSingleByChannel(
			Hit, Loc, End, FQuat::Identity, ECC_WorldStatic,
			FCollisionShape::MakeSphere(WallCheckRadius), Params);

		if (bBlocked)
		{
			// 벽 표면 법선의 수평 성분 방향으로(=벽에서 멀어지는 쪽) 새 목표를 잡는다.
			FVector Normal = Hit.ImpactNormal;
			Normal.Z = 0.0f;
			if (!Normal.IsNearlyZero())
			{
				Normal.Normalize();
				TargetRot.Yaw = Normal.Rotation().Yaw + FMath::FRandRange(-30.0f, 30.0f);
				WanderTimer = FMath::Max(WanderTimer, 1.0f); // 방금 잡은 회피 방향을 잠깐 유지
			}
		}
	}

	// 3) 위/아래: 목표 수심(WanderTargetZ)으로 향하도록 pitch 를 잡는다.
	//    (목표보다 위면 코를 올리고, 아래면 내린다. 가까워질수록 수평으로 펴짐)
	const float DepthDelta = WanderTargetZ - Loc.Z;
	TargetRot.Pitch = FMath::Clamp(DepthDelta * 0.1f, -MaxPitchDeg, MaxPitchDeg);
	TargetRot.Roll = 0.0f;

	// 4) 목표 방향으로 일정 속도로 부드럽게 회전(도망 중엔 더 빠르게)
	const FRotator NewRot = FMath::RInterpConstantTo(Owner->GetActorRotation(), TargetRot, DeltaTime, CurTurnSpeed);
	Owner->SetActorRotation(NewRot);

	// 5) 정면으로 전진(pitch 가 있으면 위/아래로도 이동). 도망 중엔 더 빠르게.
	//    스윕 끄기 — 벽 회피는 전방 트레이스가 담당.
	const FVector Forward = Owner->GetActorForwardVector();
	Owner->AddActorWorldOffset(Forward * CurSwimSpeed * DeltaTime, false);

	// 디버그: 이동 방향 화살표(메시 코를 이 방향에 맞추면 됨)
	if (bDrawForwardArrow && GetWorld())
	{
		const FVector P = Owner->GetActorLocation();
		DrawDebugDirectionalArrow(GetWorld(), P, P + Forward * 150.0f, 30.0f, FColor::Red, false, -1.0f, 0, 3.0f);
	}

	// 6) 수심대 가두기: 벗어나면 Z 위치를 범위 안으로 되돌린다(안전).
	if (bClampDepth)
	{
		FVector NewLoc = Owner->GetActorLocation();
		const float ClampedZ = FMath::Clamp(NewLoc.Z, MinZ, MaxZ);
		if (!FMath::IsNearlyEqual(ClampedZ, NewLoc.Z))
		{
			NewLoc.Z = ClampedZ;
			Owner->SetActorLocation(NewLoc);
			TargetRot.Pitch = 0.0f; // 경계에 닿으면 수평으로 펴기
		}
	}
}
