// Fill out your copyright notice in the Description page of Project Settings.

#include "Menu/MenuFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Engine/World.h"
#include "TimerManager.h"

void UMenuFunctionLibrary::StartGame(UObject* WorldContextObject, FName LevelName, bool bFade, float FadeSeconds)
{
	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull) : nullptr;
	if (!World)
	{
		return;
	}

	// 페이드 없이 즉시 이동.
	if (!bFade || FadeSeconds <= 0.0f)
	{
		UGameplayStatics::OpenLevel(World, LevelName);
		return;
	}

	// 화면을 검게 페이드 → FadeSeconds 후 OpenLevel.
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(World, 0))
	{
		if (PC->PlayerCameraManager)
		{
			// FromAlpha=0(투명) → ToAlpha=1(검정), bShouldFadeAudio=true, bHoldWhenFinished=true(이동 전까지 검은 화면 유지)
			PC->PlayerCameraManager->StartCameraFade(0.0f, 1.0f, FadeSeconds, FLinearColor::Black, true, true);
		}
	}

	// FadeSeconds 뒤 레벨 로드. 람다로 LevelName 캡처.
	FTimerHandle TimerHandle;
	TWeakObjectPtr<UWorld> WeakWorld = World;
	World->GetTimerManager().SetTimer(TimerHandle, [WeakWorld, LevelName]()
	{
		if (WeakWorld.IsValid())
		{
			UGameplayStatics::OpenLevel(WeakWorld.Get(), LevelName);
		}
	}, FadeSeconds, false);
}

void UMenuFunctionLibrary::QuitGame(UObject* WorldContextObject)
{
	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull) : nullptr;
	APlayerController* PC = World ? UGameplayStatics::GetPlayerController(World, 0) : nullptr;
	UKismetSystemLibrary::QuitGame(WorldContextObject, PC, EQuitPreference::Quit, false);
}
