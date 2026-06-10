// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "EncyclopediaSubsystem.generated.h"

/**
 * 도감 "수집 상태" 저장소.
 *
 * GameInstanceSubsystem 이라 게임이 도는 동안(레벨 이동·페이지 넘김과 무관하게) 계속 살아있다.
 * DataTable 의 IsCollected 에 의존하면 "Get Data Table Row"가 복사본을 주므로 표에 안 남고,
 * 페이지를 한 바퀴 돌면 다시 미수집으로 보인다 → 그 상태를 여기에 영구 보관해서 해결한다.
 *
 * 키는 ToCanonicalKey 로 표준화(대소문자/"BP_"접두어/번호 무시)해서 저장하므로,
 * 물고기 클래스 이름으로 등록하고 DT 행 이름으로 조회해도 서로 맞는다.
 */
UCLASS()
class SEAPROJECT_API UEncyclopediaSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:
	/** 해당 물고기를 "수집됨"으로 등록한다. (RowName 또는 물고기 이름 아무거나 — 표준키로 변환해 저장) */
	UFUNCTION(BlueprintCallable, Category = "Encyclopedia")
	void MarkCollected(FName RowName);

	/** 해당 물고기가 이미 수집됐는지. 도감 표시에서 이걸로 분기한다. */
	UFUNCTION(BlueprintPure, Category = "Encyclopedia")
	bool IsCollected(FName RowName) const;

	/** 전체 수집 초기화(새 게임 등). */
	UFUNCTION(BlueprintCallable, Category = "Encyclopedia")
	void ResetCollected();

	/** 지금까지 수집한 종 수. */
	UFUNCTION(BlueprintPure, Category = "Encyclopedia")
	int32 GetCollectedCount() const;

	// ── 보물상자(히든 퀘스트) 수집 ─────────────────────────────────────
	// 물고기와 별개의 집합. 키는 상자 액터의 고유 이름(예: "Chest_of_Gold_2")을 그대로 쓴다.
	// (물고기처럼 표준화하면 "_2" 같은 번호가 잘려 서로 다른 상자가 같은 키로 합쳐지므로 표준화하지 않는다.)

	/** 해당 상자를 "수집됨"으로 등록한다. (상자 고유 키 — 그대로 저장) */
	UFUNCTION(BlueprintCallable, Category = "Encyclopedia")
	void MarkChestCollected(FName ChestKey);

	/** 해당 상자가 이미 수집됐는지. */
	UFUNCTION(BlueprintPure, Category = "Encyclopedia")
	bool IsChestCollected(FName ChestKey) const;

	/** 지금까지 수집한 상자 수(1/4 의 분자). */
	UFUNCTION(BlueprintPure, Category = "Encyclopedia")
	int32 GetChestCollectedCount() const;

private:
	// 표준화된 키 집합(예: "shark", "clione"...)
	UPROPERTY()
	TSet<FName> CollectedKeys;

	// 수집한 상자 키 집합(상자 액터 고유 이름 그대로 저장).
	UPROPERTY()
	TSet<FName> CollectedChestKeys;
};
