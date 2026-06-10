// Fill out your copyright notice in the Description page of Project Settings.

#include "Interaction/Chest.h"
#include "Components/StaticMeshComponent.h"
#include "NiagaraComponent.h"

AChest::AChest()
{
	PrimaryActorTick.bCanEverTick = false;

	ChestMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("ChestMesh"));
	SetRootComponent(ChestMesh);

	// 완료 이펙트 컴포넌트. BP에서 이 컴포넌트에 나이아가라 에셋을 꽂고 뷰포트에서 위치/회전을 잡는다.
	// 자동재생 OFF → 평소엔 안 나오고 Collect() 에서만 1회 재생.
	CollectEffectComp = CreateDefaultSubobject<UNiagaraComponent>(TEXT("CollectEffectComp"));
	CollectEffectComp->SetupAttachment(ChestMesh);
	CollectEffectComp->SetAutoActivate(false);

	// 손전등 스캔(XRPawn)이 찾을 수 있게 "Chest" 태그를 자동 부여 → 레벨에서 태그 달 필요 없음.
	Tags.Add(FName(TEXT("Chest")));
}

void AChest::BeginPlay()
{
	Super::BeginPlay();

	// 시작부터 재생되지 않도록 확실히 꺼둔다(BP에서 Auto Activate 를 켰더라도).
	if (CollectEffectComp)
	{
		CollectEffectComp->Deactivate();
	}
}

void AChest::Collect()
{
	if (bCollected)
	{
		return; // 이미 수집됨 → 중복 이펙트/카운트 방지
	}
	bCollected = true;

	// 완료 VFX 1회 — 컴포넌트 위치/회전/크기 그대로(BP에서 잡은 대로) 재생.
	if (CollectEffectComp && CollectEffectComp->GetAsset())
	{
		CollectEffectComp->Activate(true); // 리셋 후 1회 재생
	}

	OnCollected(); // BP 추가 연출(선택)
}
