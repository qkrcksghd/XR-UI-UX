// Fill out your copyright notice in the Description page of Project Settings.

#include "Audio/BackgroundMusic.h"
#include "Components/AudioComponent.h"
#include "Sound/SoundBase.h"

ABackgroundMusic::ABackgroundMusic()
{
	PrimaryActorTick.bCanEverTick = false;

	MusicComp = CreateDefaultSubobject<UAudioComponent>(TEXT("MusicComp"));
	SetRootComponent(MusicComp);
	MusicComp->bAutoActivate = false;       // BeginPlay 에서 직접 재생
	MusicComp->bAllowSpatialization = false; // 2D BGM(위치와 무관하게 균일)
	MusicComp->bIsUISound = true;            // 게임 일시정지 등에 덜 영향받게
}

void ABackgroundMusic::BeginPlay()
{
	Super::BeginPlay();

	if (bAutoPlay)
	{
		Play();
	}
}

void ABackgroundMusic::Play()
{
	if (!Music || !MusicComp)
	{
		return;
	}

	MusicComp->SetSound(Music);
	MusicComp->SetVolumeMultiplier(Volume);

	if (FadeInSeconds > 0.0f)
	{
		MusicComp->FadeIn(FadeInSeconds, Volume);
	}
	else
	{
		MusicComp->Play();
	}
}

void ABackgroundMusic::StopMusic(float FadeOutSeconds)
{
	if (!MusicComp)
	{
		return;
	}

	if (FadeOutSeconds > 0.0f)
	{
		MusicComp->FadeOut(FadeOutSeconds, 0.0f);
	}
	else
	{
		MusicComp->Stop();
	}
}

void ABackgroundMusic::SetPaused(bool bPaused)
{
	if (MusicComp)
	{
		MusicComp->SetPaused(bPaused); // 현재 재생 위치 유지한 채 멈춤/재개
	}
}
