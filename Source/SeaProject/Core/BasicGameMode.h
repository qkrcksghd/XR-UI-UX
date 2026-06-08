// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "BasicGameMode.generated.h"

// VR 템플릿 기본 게임모드.
// 폰/컨트롤러/HUD/스테이트 등 기본 프레임워크 클래스를 "Basic" 시리즈로 묶어
// VR 템플릿 기본값과 작업자 커스텀을 이름으로 구분하기 쉽게 한다.
UCLASS()
class SEAPROJECT_API ABasicGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	ABasicGameMode();
};
