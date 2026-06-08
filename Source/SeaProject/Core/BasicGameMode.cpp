// Fill out your copyright notice in the Description page of Project Settings.

#include "Core/BasicGameMode.h"
#include "Core/BasicPlayerController.h"
#include "Core/BasicHUD.h"
#include "Core/BasicGameState.h"
#include "Core/BasicPlayerState.h"
#include "Core/BasicSpectatorPawn.h"
#include "GameFramework/Pawn.h"
#include "UObject/ConstructorHelpers.h"

ABasicGameMode::ABasicGameMode()
{
	// 기본 프레임워크 클래스들을 Basic 시리즈로 지정
	PlayerControllerClass = ABasicPlayerController::StaticClass();
	HUDClass              = ABasicHUD::StaticClass();
	GameStateClass        = ABasicGameState::StaticClass();
	PlayerStateClass      = ABasicPlayerState::StaticClass();
	SpectatorClass        = ABasicSpectatorPawn::StaticClass();

	// 기본 폰 = BP_XRPawn (작업자 폰). 경로가 맞으면 자동 지정되어 Play 시 이 폰이 스폰된다.
	// (BP_XRPawn 을 옮기거나 이름 바꾸면 이 경로도 같이 고쳐야 함)
	static ConstructorHelpers::FClassFinder<APawn> PawnBPClass(TEXT("/Game/CH/BP_XRPawn"));
	if (PawnBPClass.Succeeded())
	{
		DefaultPawnClass = PawnBPClass.Class;
	}
}
