// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "ScannableInterface.generated.h"

// 손전등 스캔으로 인식 가능한 액터(물고기 등)가 구현하는 인터페이스.
// BP_FishBase 에서 이 인터페이스를 추가하고 함수들을 구현하면 된다.
UINTERFACE(MinimalAPI, Blueprintable)
class UScannableInterface : public UInterface
{
	GENERATED_BODY()
};

class IScannableInterface
{
	GENERATED_BODY()

public:
	// 이 물고기가 도감 데이터테이블(DT_Encyclopedia)에서 어느 행(RowName)인지 반환.
	// BP_FishBase 에 이미 있는 도감 ID 변수를 그대로 돌려주면 된다.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Scan")
	FName GetEncyclopediaRowName() const;
	virtual FName GetEncyclopediaRowName_Implementation() const { return NAME_None; }

	// 스캔 진행 중 매 프레임 호출 (Progress01: 0.0 ~ 1.0). 진행바/연출용.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Scan")
	void OnScanProgress(float Progress01);
	virtual void OnScanProgress_Implementation(float Progress01) {}

	// 스캔이 완료되었을 때 호출. 여기서 도감 등록/홀로그램 표시를 하면 된다.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Scan")
	void OnScanCompleted();
	virtual void OnScanCompleted_Implementation() {}

	// 완료 전에 빛이 벗어나 스캔이 취소되었을 때 호출.
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Scan")
	void OnScanCancelled();
	virtual void OnScanCancelled_Implementation() {}
};
