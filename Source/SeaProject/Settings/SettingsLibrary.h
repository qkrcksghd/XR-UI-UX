// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "SettingsLibrary.generated.h"

// 설정 메뉴 슬라이더 연결용. 값은 엔진 전역에 적용되므로 레벨이 바뀌어도 유지된다.
//  - 볼륨 슬라이더 OnValueChanged → SetMasterVolume (BGM 포함 모든 소리)
//  - 밝기 슬라이더 OnValueChanged → SetBrightness
//  - 슬라이더 초기값은 Get... 으로 맞춘다.
UCLASS()
class SEAPROJECT_API USettingsLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// 마스터 볼륨(0~1). 게임 내 모든 사운드에 곱해지는 전역 배율.
	UFUNCTION(BlueprintCallable, Category = "Settings|Audio", meta = (WorldContext = "WorldContextObject"))
	static void SetMasterVolume(UObject* WorldContextObject, float Volume);

	// 현재 마스터 볼륨(0~1) — 슬라이더 초기값용.
	UFUNCTION(BlueprintPure, Category = "Settings|Audio")
	static float GetMasterVolume();

	// 화면 밝기(0~1). 0=어둡게, 1=밝게. 내부적으로 디스플레이 감마를 조절.
	UFUNCTION(BlueprintCallable, Category = "Settings|Display")
	static void SetBrightness(float Brightness);

	// 현재 밝기(0~1) — 슬라이더 초기값용.
	UFUNCTION(BlueprintPure, Category = "Settings|Display")
	static float GetBrightness();
};
