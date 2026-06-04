// Fill out your copyright notice in the Description page of Project Settings.

#include "FlashlightScanComponent.h"
#include "ScannableInterface.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "Components/SceneComponent.h"
#include "CollisionQueryParams.h"
#include "UObject/UnrealType.h"
#include "GameFramework/Actor.h"
#include "DrawDebugHelpers.h"

UFlashlightScanComponent::UFlashlightScanComponent()
{
	PrimaryComponentTick.bCanEverTick = true;
}

void UFlashlightScanComponent::SetTraceSource(USceneComponent* InSource)
{
	TraceSource = InSource;
}

void UFlashlightScanComponent::SetScanningActive(bool bActive)
{
	bScanningActive = bActive;
	if (!bActive && CurrentTarget)
	{
		CancelCurrent();
	}
}

float UFlashlightScanComponent::GetScanDurationForRow(FName RowName) const
{
	if (!EncyclopediaTable || RowName.IsNone())
	{
		return DefaultScanDuration;
	}

	const uint8* RowData = EncyclopediaTable->FindRowUnchecked(RowName);
	const UScriptStruct* RowStruct = EncyclopediaTable->GetRowStruct();
	if (!RowData || !RowStruct)
	{
		return DefaultScanDuration;
	}

	const FString TargetName = ScanDurationColumn.ToString();
	for (TFieldIterator<FProperty> It(RowStruct); It; ++It)
	{
		FProperty* Prop = *It;
		// 블루프린트 구조체 멤버는 내부 이름에 GUID 가 붙으므로 AuthoredName(원래 이름)으로 비교.
		if (Prop->GetAuthoredName() != TargetName && Prop->GetName() != TargetName)
		{
			continue;
		}

		// UE5 의 블루프린트 float 는 내부적으로 double 인 경우가 많아 둘 다 처리.
		if (const FDoubleProperty* DP = CastField<FDoubleProperty>(Prop))
		{
			return static_cast<float>(DP->GetPropertyValue_InContainer(RowData));
		}
		if (const FFloatProperty* FP = CastField<FFloatProperty>(Prop))
		{
			return FP->GetPropertyValue_InContainer(RowData);
		}
	}

	return DefaultScanDuration;
}

void UFlashlightScanComponent::CancelCurrent()
{
	if (CurrentTarget && !bCompletedForTarget)
	{
		if (CurrentTarget->Implements<UScannableInterface>())
		{
			IScannableInterface::Execute_OnScanCancelled(CurrentTarget);
		}
		OnScanCancelled.Broadcast();
	}

	if (bResetProgressOnLoseTarget)
	{
		Accum = 0.0f;
		CurrentProgress = 0.0f;
	}
	CurrentTarget = nullptr;
	CurrentRowName = NAME_None;
	bCompletedForTarget = false;
	bIsScanning = false;
}

void UFlashlightScanComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (!bScanningActive || !TraceSource)
	{
		if (CurrentTarget)
		{
			CancelCurrent();
		}
		return;
	}

	UWorld* World = GetWorld();
	if (!World)
	{
		return;
	}

	const FVector Start = TraceSource->GetComponentLocation();
	const FVector End = Start + TraceSource->GetForwardVector() * ScanRange;

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(FlashlightScan), false, GetOwner());

	bool bHit;
	if (ScanRadius > KINDA_SMALL_NUMBER)
	{
		bHit = World->SweepSingleByChannel(Hit, Start, End, FQuat::Identity, TraceChannel,
			FCollisionShape::MakeSphere(ScanRadius), Params);
	}
	else
	{
		bHit = World->LineTraceSingleByChannel(Hit, Start, End, TraceChannel, Params);
	}

	AActor* HitFish = nullptr;
	if (bHit)
	{
		AActor* HitActor = Hit.GetActor();
		if (HitActor && HitActor->Implements<UScannableInterface>())
		{
			HitFish = HitActor;
		}
	}

#if ENABLE_DRAW_DEBUG
	if (bDrawDebug)
	{
		const FVector DebugEnd = bHit ? Hit.ImpactPoint : End;
		DrawDebugLine(World, Start, DebugEnd, HitFish ? FColor::Green : FColor::Red, false, -1.0f, 0, 1.0f);
	}
#endif

	if (HitFish)
	{
		// 새 대상으로 바뀌면 이전 대상 정리 후 새로 시작
		if (HitFish != CurrentTarget)
		{
			CancelCurrent();

			CurrentTarget = HitFish;
			Accum = 0.0f;
			bCompletedForTarget = false;
			CurrentRowName = IScannableInterface::Execute_GetEncyclopediaRowName(HitFish);
			CurrentDuration = GetScanDurationForRow(CurrentRowName);
			if (CurrentDuration <= 0.0f)
			{
				CurrentDuration = DefaultScanDuration;
			}
			// 0/음수면 0으로 나누는 사고가 나므로 최소값 보장
			CurrentDuration = FMath::Max(CurrentDuration, 0.01f);

			bIsScanning = true;
			OnScanStarted.Broadcast(CurrentTarget, CurrentRowName);
		}

		if (!bCompletedForTarget)
		{
			Accum += DeltaTime;
			CurrentProgress = FMath::Clamp(Accum / CurrentDuration, 0.0f, 1.0f);
			bIsScanning = true;

			IScannableInterface::Execute_OnScanProgress(CurrentTarget, CurrentProgress);
			OnScanProgress.Broadcast(CurrentProgress);

			if (Accum >= CurrentDuration)
			{
				bCompletedForTarget = true;
				bIsScanning = false;

				IScannableInterface::Execute_OnScanCompleted(CurrentTarget);
				OnScanCompleted.Broadcast(CurrentTarget, CurrentRowName);
			}
		}
		else
		{
			// 이미 완료된 대상을 계속 비추는 경우 — 재발동하지 않음
			bIsScanning = false;
		}
	}
	else
	{
		// 물고기를 못 맞춤 → 진행 중이던 스캔 취소
		if (CurrentTarget)
		{
			CancelCurrent();
		}
	}
}
