// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BackgroundMusic.generated.h"

class UAudioComponent;
class USoundBase;

// 레벨 BGM 재생용 액터. 레벨에 1개 배치하고 Music 에 곡을 지정하면
// 시작 시 자동으로(페이드 인) 재생한다. 레벨이 바뀌면 액터와 함께 사라지므로
// 이전 곡은 자동 정지되고 새 레벨의 BGM이 시작된다.
//  - MainMenu 레벨 → 메뉴 BGM
//  - 게임 맵(VRTemplateMap 등) → 게임 BGM
// (반복 재생은 Music 사운드 에셋의 Looping 을 켜야 한다 — 아래 안내 참고)
UCLASS()
class SEAPROJECT_API ABackgroundMusic : public AActor
{
	GENERATED_BODY()

public:
	ABackgroundMusic();

protected:
	virtual void BeginPlay() override;

public:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Music")
	UAudioComponent* MusicComp;

	// 재생할 BGM (Sound Wave 또는 Sound Cue). 반복하려면 에셋의 Looping 을 켤 것.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	USoundBase* Music;

	// 시작 시 자동 재생.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	bool bAutoPlay = true;

	// 이 트랙 자체 볼륨(마스터 볼륨과 별개). 0~1 권장.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music", meta = (ClampMin = "0.0"))
	float Volume = 1.0f;

	// 페이드 인 시간(초). 0이면 즉시 재생.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Music")
	float FadeInSeconds = 1.5f;

	// 곡 재생 시작.
	UFUNCTION(BlueprintCallable, Category = "Music")
	void Play();

	// 곡 정지(페이드 아웃).
	UFUNCTION(BlueprintCallable, Category = "Music")
	void StopMusic(float FadeOutSeconds = 1.0f);

	// 일시정지/재개. true 면 현재 위치에서 멈추고, false 면 그 위치부터 이어서 재생한다.
	// (스캔 중 BGM 을 잠깐 죽였다가 끝나면 이어 듣게 하는 용도)
	UFUNCTION(BlueprintCallable, Category = "Music")
	void SetPaused(bool bPaused);
};
