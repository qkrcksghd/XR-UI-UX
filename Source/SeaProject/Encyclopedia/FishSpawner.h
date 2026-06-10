// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FishSpawner.generated.h"

class UDataTable;

/**
 * 도감(DT_Encyclopedia) 기반 물고기 자동 스포너.
 *
 * 레벨에 1개 배치하고 EncyclopediaTable / FishClass 를 지정하면, BeginPlay 에서 표의 모든 행을 돌며
 * 각 종의 Depth(수심, m) 를 Z 로 변환해 물고기를 스폰한다.
 *   Z = SurfaceZ - Depth(m) * UnitsPerMeter
 *
 * 스폰된 물고기는 손전등 스캔이 찾을 수 있게 "Fish" 태그가 붙고, 어떤 종인지 알 수 있게
 * FishRowName 프로퍼티(있으면)에 행 이름이 각인된다. 메시/애니는 표의 ModelMesh/AnimationSequence 를
 * C++ 가 직접 SkeletalMeshComponent 에 꽂는다.
 */
UCLASS()
class SEAPROJECT_API AFishSpawner : public AActor
{
	GENERATED_BODY()

public:
	AFishSpawner();

protected:
	virtual void BeginPlay() override;

public:
	// ── 데이터 / 스폰 대상 ────────────────────────────────────────────────
	/** 도감 데이터테이블(DT_Encyclopedia). 여기 행을 돌며 스폰한다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSpawner")
	UDataTable* EncyclopediaTable;

	/** 종별로 소환할 블루프린트. Key=행 이름(대소문자/BP_ 무시), Value=BP 클래스(예: shark→BP_Shark).
	 *  여기 지정된 종은 그 BP 를 그대로 소환한다 — 메시/애니/방향은 BP 에 이미 설정된 걸 그대로 쓴다(C++ 가 안 건드림).
	 *  스포너는 위치(수심)/마리수/태그/행이름만 담당. 가장 깔끔한 방식. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSpawner")
	TMap<FName, TSubclassOf<AActor>> SpeciesBlueprints;

	/** 위 맵에 없는 종에 쓸 폴백 클래스. 비워두면 SkeletalMeshComponent 하나짜리 기본 액터를 만들어 DT 의 메시/애니를 C++ 가 꽂는다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSpawner")
	TSubclassOf<AActor> FishClass;

	// ── 수심 → Z 변환 (수면 Z=0 기준) ──────────
	/** 수심 0(해수면)에 해당하는 Z 값. (bSpawnerZIsSurface 가 켜져 있으면 무시되고 스포너 Z 를 씀) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSpawner|Depth")
	float SurfaceZ = 0.0f;

	/** 켜면 SurfaceZ 대신 "이 스포너를 놓은 Z"를 수면(수심0)으로 쓴다.
	 *  → 스포너를 보이는 위치(예 잠수함 근처)에 놓으면 물고기가 바로 그 아래로 깊이대로 퍼져 눈에 보인다.
	 *  레벨 스케일이 가정(34849유닛=10977m)과 다를 때 이걸 켜는 게 편하다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSpawner|Depth")
	bool bSpawnerZIsSurface = false;

	/** 실제 1미터당 언리얼 Z 유닛 수. (1번 기준 34849유닛/10977m ≈ 3.175) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSpawner|Depth")
	float UnitsPerMeter = 3.175f;

	// ── 배치 ──────────────────────────────────────────────────────────────
	/** 스포너 위치를 중심으로 한 XY 랜덤 배치 반경(유닛). Z 는 수심으로 결정됨. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSpawner|Placement")
	FVector2D SpawnAreaExtent = FVector2D(5000.0f, 5000.0f);

	/** 종(행)당 기본 스폰 마리 수. 아래 SpawnCountPerSpecies 에 없는 종은 이 값을 쓴다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSpawner|Placement")
	int32 CountPerSpecies = 1;

	/** 종별 마리 수 개별 지정. Key=행 이름(대소문자/BP_접두어 무시), Value=마리 수.
	 *  예: shark=1, sardine=10. 여기 있는 종은 이 값, 없는 종은 CountPerSpecies. 0 으로 두면 그 종은 안 나옴. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSpawner|Placement")
	TMap<FName, int32> SpawnCountPerSpecies;

	/** 환경 메시(지형/바위 등) 안에 박혀 스폰되지 않도록, 빈 공간을 찾아서 스폰한다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSpawner|Placement")
	bool bAvoidEnvironment = true;

	/** 물고기가 차지할 여유 반경(유닛). 이 반경의 구체가 환경과 겹치면 다른 XY 를 다시 뽑는다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSpawner|Placement")
	float SpawnClearRadius = 100.0f;

	/** 빈 공간을 찾기 위한 최대 재시도 횟수. 다 실패하면 그 마리는 건너뛴다(환경에 안 박히게). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSpawner|Placement")
	int32 MaxPlacementTries = 12;

	/** 스폰에서 제외할 행 이름들(대소문자/접두어 무시). 기본값: jellyfish. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSpawner|Placement")
	TArray<FName> ExcludeRows;

	/** 화이트리스트: 비어있지 않으면 "이 행들만" 스폰한다(나머지는 전부 스킵).
	 *  예: 깊은 협곡 전용 스포너에 [lure, shark] 만 넣으면 그 둘만 여기서 생성.
	 *  비워두면(기본) 제외목록 빼고 전부 스폰. ExcludeRows 와 같이 쓰면 둘 다 적용(포함 ∧ 비제외). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSpawner|Placement")
	TArray<FName> IncludeOnlyRows;

	/** BeginPlay 에 자동으로 전부 스폰할지. 끄면 SpawnAllFish 를 직접 호출. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSpawner")
	bool bSpawnOnBeginPlay = true;

	/** 스폰 디버그: 물고기마다 메시/애니/스켈레톤 일치 여부를 화면에 표시. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSpawner")
	bool bDebugSpawn = true;

	// ── 헤엄 (FishSwimComponent 자동 부착) ────────────────────────────────
	/** 스폰한 물고기에 헤엄+벽회피 컴포넌트를 자동으로 붙인다. (켜면 BP_FishBase 의 기존 이동 로직은 꺼야 함!) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSpawner|Swim")
	bool bAddSwimComponent = true;

	/** 헤엄 속도(cm/s). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSpawner|Swim")
	float SwimSpeed = 120.0f;

	/** 메시 정면 보정(모든 종 공통, Pitch/Yaw/Roll 도). 물고기가 옆/뒤/위로 헤엄치는 것처럼 보이면 돌려 맞춘다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSpawner|Swim")
	FRotator MeshRotationOffset = FRotator::ZeroRotator;

	/** 종별 메시 정면 보정(이 맵에 있으면 위 공통값 대신 사용). 예: lure 만 (0,90,0) 처럼 따로 돌릴 때.
	 *  키 = 행 이름(대소문자/접두어 무시), 값 = Pitch/Yaw/Roll(도). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSpawner|Swim")
	TMap<FName, FRotator> MeshRotationPerSpecies;

	/** 각 물고기를 자기 종의 수심대(minDepth~maxDepth → Z) 안에 머물게 한다. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "FishSpawner|Swim")
	bool bClampToDepthBand = true;

	/** 표의 모든 행(제외 목록 빼고)을 돌며 물고기를 스폰한다. 스폰한 총 마리 수 반환. */
	UFUNCTION(BlueprintCallable, Category = "FishSpawner")
	int32 SpawnAllFish();

private:
	// 행 하나를 지정 위치에 스폰하고 메시/애니/태그/행이름을 세팅. 실패 시 nullptr.
	AActor* SpawnOneFish(FName RowName, FVector Location);

	// 주어진 수심 Z 에서 환경과 안 겹치는 XY 위치를 찾는다. 못 찾으면 false.
	bool FindClearLocation(float TargetZ, FVector& OutLocation) const;

	// 제외 목록에 포함되는 행인지(표준키 비교).
	bool IsExcluded(FName RowName) const;

	// 이 스포너가 이 행을 스폰해야 하는지(IncludeOnlyRows 비었으면 항상 true, 아니면 목록에 있을 때만).
	bool IsIncluded(FName RowName) const;

	// 이 행에 쓸 메시 정면 보정 회전(종별 지정 있으면 그 값, 없으면 공통 MeshRotationOffset).
	FRotator GetMeshRotationForRow(FName RowName) const;

	// 이 행을 몇 마리 스폰할지(개별 지정 있으면 그 값, 없으면 CountPerSpecies).
	int32 GetSpawnCountForRow(FName RowName) const;

	// 이 행에 쓸 액터 클래스. 종별 BP 가 있으면 그걸(bOutIsSpeciesBP=true), 없으면 FishClass.
	TSubclassOf<AActor> GetFishClassForRow(FName RowName, bool& bOutIsSpeciesBP) const;

	// 수심 0 기준이 되는 Z (옵션에 따라 스포너 Z 또는 SurfaceZ).
	float GetEffectiveSurfaceZ() const;

	// 중복 스폰 방지.
	bool bHasSpawned = false;
};
