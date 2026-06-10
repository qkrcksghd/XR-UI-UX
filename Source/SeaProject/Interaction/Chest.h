// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Chest.generated.h"

class UStaticMeshComponent;
class UNiagaraComponent;

/**
 * 보물상자 (히든 퀘스트). 손전등으로 스캔 완료하면 수집된다.
 *  - 생성자에서 "Chest" 태그를 자동 부여 → 레벨에서 태그를 따로 달 필요가 없다.
 *  - 완료 이펙트는 CollectEffectComp(나이아가라 컴포넌트)에 에셋을 직접 지정하고,
 *    뷰포트에서 그 컴포넌트를 옮겨/돌려 위치·방향을 잡는다. 평소엔 꺼져 있고 Collect() 에서만 재생.
 *  - 카운트(1/4)는 XRPawn 이 집계해 팝업으로 띄운다(이 액터는 자기 연출만 담당).
 *
 * 사용법: 이 클래스로 BP_Chest 를 만들어 ChestMesh 에 SM Chest_of_Gold, CollectEffectComp 에
 *         나이아가라 에셋을 지정한 뒤 레벨에 4개 배치.
 */
UCLASS()
class SEAPROJECT_API AChest : public AActor
{
	GENERATED_BODY()

public:
	AChest();

protected:
	virtual void BeginPlay() override;

public:
	// 상자 메쉬(BP_Chest 에서 SM Chest_of_Gold 지정). 루트 컴포넌트.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chest")
	UStaticMeshComponent* ChestMesh;

	// 완료 이펙트. 이 컴포넌트의 "Niagara System Asset" 에 나이아가라를 직접 지정하고,
	// 뷰포트에서 이 컴포넌트를 옮겨/돌려 이펙트 위치·회전·크기를 마음대로 맞춘다.
	// 평소엔 꺼져 있고(자동재생 OFF) Collect() 시점에만 그 위치에서 1회 재생.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Chest")
	UNiagaraComponent* CollectEffectComp;

	// 이 상자를 스캔하는 데 걸리는 시간(초). 0 이하면 폰의 ChestScanDuration 을 쓴다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Chest")
	float ScanTime = 2.0f;

	// 수집 처리: CollectEffectComp 1회 재생 + bCollected=true + BP 추가연출(OnCollected) 호출.
	// 이미 수집됐으면 아무것도 안 한다(중복 방지).
	UFUNCTION(BlueprintCallable, Category = "Chest")
	void Collect();

	// 이미 수집된 상자인지.
	UFUNCTION(BlueprintPure, Category = "Chest")
	bool IsCollected() const { return bCollected; }

protected:
	// 수집 완료 직후 1회 호출 → BP에서 상자별 추가 연출(뚜껑 열기/메쉬 교체/사라지기 등)을 하고 싶을 때.
	UFUNCTION(BlueprintImplementableEvent, Category = "Chest")
	void OnCollected();

private:
	bool bCollected = false;
};
