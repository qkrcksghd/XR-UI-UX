// Fill out your copyright notice in the Description page of Project Settings.

#include "FishSpawner.h"
#include "EncyclopediaLibrary.h"
#include "FishSwimComponent.h"
#include "Engine/DataTable.h"
#include "Engine/World.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimSequenceBase.h"
#include "Components/SkeletalMeshComponent.h"
#include "CollisionQueryParams.h"

AFishSpawner::AFishSpawner()
{
	PrimaryActorTick.bCanEverTick = false;

	// 기본 제외: jellyfish
	ExcludeRows.Add(FName(TEXT("jellyfish")));
}

void AFishSpawner::BeginPlay()
{
	Super::BeginPlay();

	if (bSpawnOnBeginPlay)
	{
		SpawnAllFish();
	}
}

bool AFishSpawner::IsExcluded(FName RowName) const
{
	const FName Key = UEncyclopediaLibrary::ToCanonicalKey(RowName);
	for (const FName& Ex : ExcludeRows)
	{
		if (UEncyclopediaLibrary::ToCanonicalKey(Ex) == Key)
		{
			return true;
		}
	}
	return false;
}

float AFishSpawner::GetEffectiveSurfaceZ() const
{
	return bSpawnerZIsSurface ? GetActorLocation().Z : SurfaceZ;
}

TSubclassOf<AActor> AFishSpawner::GetFishClassForRow(FName RowName, bool& bOutIsSpeciesBP) const
{
	const FName Key = UEncyclopediaLibrary::ToCanonicalKey(RowName);
	for (const TPair<FName, TSubclassOf<AActor>>& Pair : SpeciesBlueprints)
	{
		if (Pair.Value && UEncyclopediaLibrary::ToCanonicalKey(Pair.Key) == Key)
		{
			bOutIsSpeciesBP = true;
			return Pair.Value;
		}
	}
	bOutIsSpeciesBP = false;
	return FishClass;
}

int32 AFishSpawner::GetSpawnCountForRow(FName RowName) const
{
	// 1순위: DT 의 SpawnCount 필드(데이터로 관리). 필드가 있고 값이 있으면 그걸 쓴다.
	int32 DtCount = 0;
	if (EncyclopediaTable && UEncyclopediaLibrary::GetRowSpawnCount(EncyclopediaTable, RowName, DtCount))
	{
		return FMath::Max(0, DtCount);
	}

	// 2순위: 스포너의 종별 개별 지정 Map.
	const FName Key = UEncyclopediaLibrary::ToCanonicalKey(RowName);
	for (const TPair<FName, int32>& Pair : SpawnCountPerSpecies)
	{
		if (UEncyclopediaLibrary::ToCanonicalKey(Pair.Key) == Key)
		{
			return FMath::Max(0, Pair.Value);
		}
	}

	// 3순위: 전체 기본값.
	return FMath::Max(0, CountPerSpecies);
}

int32 AFishSpawner::SpawnAllFish()
{
	if (bHasSpawned)
	{
		return 0;
	}
	if (!EncyclopediaTable || !GetWorld())
	{
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Red, TEXT("[FishSpawner] EncyclopediaTable 미지정"));
		}
		return 0;
	}

	bHasSpawned = true;

	int32 SpawnedCount = 0;
	int32 SkippedCount = 0;
	const TArray<FName> RowNames = EncyclopediaTable->GetRowNames();
	for (const FName& Row : RowNames)
	{
		if (IsExcluded(Row))
		{
			continue;
		}

		// 수심 범위(minDepth~maxDepth). min/max 없으면 단일 Depth 로 폴백(Min==Max), 그것도 없으면 0(수면).
		float MinDepth = 0.0f, MaxDepth = 0.0f;
		UEncyclopediaLibrary::GetRowDepthRange(EncyclopediaTable, Row, MinDepth, MaxDepth);

		const int32 Count = GetSpawnCountForRow(Row);
		for (int32 i = 0; i < Count; ++i)
		{
			// 마리마다 범위 안 랜덤 수심 → 같은 종도 수심대에 자연스럽게 흩어진다.
			const float DepthM = FMath::FRandRange(MinDepth, MaxDepth);
			const float TargetZ = GetEffectiveSurfaceZ() - DepthM * UnitsPerMeter;

			FVector Loc;
			if (!FindClearLocation(TargetZ, Loc))
			{
				// 그 수심대 빈 공간을 못 찾음(환경에 막힘) → 박히지 않게 건너뜀
				++SkippedCount;
				continue;
			}

			if (SpawnOneFish(Row, Loc))
			{
				++SpawnedCount;
			}
		}
	}

	if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 5.0f, FColor::Cyan,
			FString::Printf(TEXT("[FishSpawner] 물고기 %d마리 스폰 (건너뜀 %d)"), SpawnedCount, SkippedCount));
	}
	return SpawnedCount;
}

bool AFishSpawner::FindClearLocation(float TargetZ, FVector& OutLocation) const
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return false;
	}

	const int32 Tries = FMath::Max(1, MaxPlacementTries);
	for (int32 t = 0; t < Tries; ++t)
	{
		const float OffsetX = FMath::FRandRange(-SpawnAreaExtent.X, SpawnAreaExtent.X);
		const float OffsetY = FMath::FRandRange(-SpawnAreaExtent.Y, SpawnAreaExtent.Y);
		FVector Loc = GetActorLocation() + FVector(OffsetX, OffsetY, 0.0f);
		Loc.Z = TargetZ;

		if (!bAvoidEnvironment)
		{
			OutLocation = Loc;
			return true;
		}

		// 이 위치에 SpawnClearRadius 구체가 환경(정적/동적 메시)과 겹치는지 검사
		FCollisionObjectQueryParams ObjParams;
		ObjParams.AddObjectTypesToQuery(ECC_WorldStatic);
		ObjParams.AddObjectTypesToQuery(ECC_WorldDynamic);

		FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(FishSpawnClearCheck), false, this);

		const bool bBlocked = World->OverlapAnyTestByObjectType(
			Loc, FQuat::Identity, ObjParams,
			FCollisionShape::MakeSphere(SpawnClearRadius), QueryParams);

		if (!bBlocked)
		{
			OutLocation = Loc;
			return true;
		}
	}

	return false; // 전부 막힘
}

AActor* AFishSpawner::SpawnOneFish(FName RowName, FVector Location)
{
	UWorld* World = GetWorld();
	if (!World)
	{
		return nullptr;
	}

	FActorSpawnParameters Params;
	Params.Owner = this;
	Params.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

	// 랜덤 방향으로 스폰(다 같은 방향 보지 않게)
	const FRotator SpawnRot(0.0f, FMath::FRandRange(0.0f, 360.0f), 0.0f);
	const FTransform SpawnTM(SpawnRot, Location);

	// 이 종에 쓸 클래스 결정: 종별 BP(있으면) → 없으면 FishClass → 없으면 기본 액터
	bool bSpeciesBP = false;
	const TSubclassOf<AActor> ClassToSpawn = GetFishClassForRow(RowName, bSpeciesBP);

	AActor* Spawned = nullptr;
	if (ClassToSpawn)
	{
		Spawned = World->SpawnActor<AActor>(ClassToSpawn, SpawnTM, Params);
	}
	else
	{
		Spawned = World->SpawnActor<AActor>(AActor::StaticClass(), SpawnTM, Params);
	}

	if (!Spawned)
	{
		return nullptr;
	}

	// 종별 BP 면 메시/애니/방향은 BP 에 이미 설정돼 있으니 C++ 는 안 건드린다.
	// 종별 BP 가 아닐 때만(제네릭/기본) DT 의 ModelMesh/AnimationSequence 를 C++ 가 꽂는다.
	if (!bSpeciesBP)
	{
		USkeletalMeshComponent* SkComp = Spawned->FindComponentByClass<USkeletalMeshComponent>();
		if (!SkComp)
		{
			SkComp = NewObject<USkeletalMeshComponent>(Spawned, TEXT("FishMesh"));
			if (SkComp)
			{
				Spawned->SetRootComponent(SkComp);
				SkComp->RegisterComponent();
				SkComp->SetWorldTransform(SpawnTM);
			}
		}

		if (SkComp)
		{
			USkeletalMesh* Mesh = UEncyclopediaLibrary::GetRowModelMesh(EncyclopediaTable, RowName);
			UAnimSequenceBase* Anim = UEncyclopediaLibrary::GetRowAnimSequence(EncyclopediaTable, RowName);

			if (Mesh)
			{
				SkComp->SetSkeletalMeshAsset(Mesh);
			}
			if (Anim)
			{
				SkComp->SetAnimationMode(EAnimationMode::AnimationSingleNode);
				SkComp->PlayAnimation(Anim, true); // 루프 재생
			}
		}
	}

	if (bDebugSpawn && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 10.0f, FColor::Cyan,
			FString::Printf(TEXT("[%s] %s"), *RowName.ToString(),
				bSpeciesBP ? *FString::Printf(TEXT("BP=%s"), *Spawned->GetClass()->GetName()) : TEXT("(generic mesh)")));
	}

	// 스캔이 찾을 수 있게 태그 + 어떤 종인지 각인
	Spawned->Tags.AddUnique(FName(TEXT("Fish")));
	UEncyclopediaLibrary::SetActorFishRowName(Spawned, RowName);

	// 헤엄+벽회피 컴포넌트 자동 부착
	if (bAddSwimComponent)
	{
		if (UFishSwimComponent* Swim = NewObject<UFishSwimComponent>(Spawned, TEXT("FishSwim")))
		{
			Swim->SwimSpeed = SwimSpeed;
			Swim->MeshYawOffset = MeshYawOffset;

			// 이 종의 수심대(Z 범위) 안에 가두기
			if (bClampToDepthBand)
			{
				float MinDepth = 0.0f, MaxDepth = 0.0f;
				if (UEncyclopediaLibrary::GetRowDepthRange(EncyclopediaTable, RowName, MinDepth, MaxDepth))
				{
					const float Surf = GetEffectiveSurfaceZ();
					Swim->bClampDepth = true;
					Swim->MinZ = Surf - MaxDepth * UnitsPerMeter; // 더 깊음 = 더 낮은 Z
					Swim->MaxZ = Surf - MinDepth * UnitsPerMeter; // 더 얕음 = 더 높은 Z
				}
			}

			Swim->RegisterComponent();
		}
	}

	return Spawned;
}
