// Fill out your copyright notice in the Description page of Project Settings.

#include "Settings/SettingsLibrary.h"
#include "Engine/Engine.h"
#include "Engine/World.h"
#include "AudioDevice.h"

// 세션 동안 유지되는 캐시값(슬라이더 초기값 복원용). 레벨이 바뀌어도 UClass 정적이라 살아있다.
static float GCachedMasterVolume = 1.0f;
static float GCachedBrightness = 0.6f; // 0.6 → 기본 감마(약 2.2)

void USettingsLibrary::SetMasterVolume(UObject* WorldContextObject, float Volume)
{
	Volume = FMath::Clamp(Volume, 0.0f, 1.0f);
	GCachedMasterVolume = Volume;

	UWorld* World = GEngine ? GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull) : nullptr;
	if (World)
	{
		// 오디오 디바이스의 전역 배율 → 모든 사운드(BGM/효과음)에 적용. 레벨 로드 후에도 디바이스는 유지됨.
		if (FAudioDevice* AudioDevice = World->GetAudioDeviceRaw())
		{
			AudioDevice->SetTransientPrimaryVolume(Volume);
		}
	}
}

float USettingsLibrary::GetMasterVolume()
{
	return GCachedMasterVolume;
}

void USettingsLibrary::SetBrightness(float Brightness)
{
	Brightness = FMath::Clamp(Brightness, 0.0f, 1.0f);
	GCachedBrightness = Brightness;

	if (GEngine)
	{
		// 슬라이더 1=밝게, 0=어둡게 → 감마 1.8(밝음) ~ 2.8(어두움). 기본 2.2 ≈ Brightness 0.6.
		GEngine->DisplayGamma = FMath::Lerp(2.8f, 1.8f, Brightness);
	}
}

float USettingsLibrary::GetBrightness()
{
	return GCachedBrightness;
}
