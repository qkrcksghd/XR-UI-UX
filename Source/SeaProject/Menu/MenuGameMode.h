// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MenuGameMode.generated.h"

// 시작 화면(메인 메뉴) 레벨 전용 게임모드.
// 기본 폰을 메뉴 폰(BP_MenuPawn)으로 지정해, 메뉴 레벨에서는 레이저 포인터 폰이 스폰되게 한다.
// MainMenu 레벨의 World Settings → GameMode Override 에 이 클래스를 지정한다.
UCLASS()
class SEAPROJECT_API AMenuGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	AMenuGameMode();
};
