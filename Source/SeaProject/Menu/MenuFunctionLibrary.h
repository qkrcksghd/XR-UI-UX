// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "MenuFunctionLibrary.generated.h"

// 시작 화면(메인 메뉴) 버튼에서 호출하는 헬퍼 모음.
// WBP_MainMenu 의 버튼 OnClicked 이벤트에서 이 노드들을 연결한다.
//  - 게임 시작 버튼 → StartGame (바다 맵으로 이동, VR 멀미 방지 페이드 옵션)
//  - 종료 버튼     → QuitGame
//  - 설정은 위젯 토글이라 C++ 불필요 (WBP 안에서 설정 패널 Visible 토글)
UCLASS()
class SEAPROJECT_API UMenuFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	// 게임 맵으로 이동한다. LevelName 은 맵 이름(예: "VRTemplateMap") 또는
	// 전체 경로("/Game/VRTemplate/VRTemplateMap") 둘 다 가능.
	// bFade=true 면 화면을 검게 페이드한 뒤 FadeSeconds 후 OpenLevel (VR 멀미/깜빡임 완화).
	UFUNCTION(BlueprintCallable, Category = "Menu", meta = (WorldContext = "WorldContextObject", AdvancedDisplay = "bFade,FadeSeconds"))
	static void StartGame(UObject* WorldContextObject, FName LevelName = TEXT("VRTemplateMap"), bool bFade = true, float FadeSeconds = 0.6f);

	// 게임 종료 (에디터 PIE 에서는 플레이 정지).
	UFUNCTION(BlueprintCallable, Category = "Menu", meta = (WorldContext = "WorldContextObject"))
	static void QuitGame(UObject* WorldContextObject);
};
