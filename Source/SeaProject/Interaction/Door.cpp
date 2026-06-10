// Fill out your copyright notice in the Description page of Project Settings.

#include "Interaction/Door.h"
#include "Components/StaticMeshComponent.h"
#include "Components/SphereComponent.h"
#include "Components/TextRenderComponent.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/SoundBase.h"
#include "Engine/Font.h"
#include "Engine/World.h"
#include "Materials/MaterialInterface.h"
#include "TimerManager.h"

ADoor::ADoor()
{
	PrimaryActorTick.bCanEverTick = true;

	RootScene = CreateDefaultSubobject<USceneComponent>(TEXT("RootScene"));
	SetRootComponent(RootScene);

	// 문 패널 — 디테일에서 SM_MetalDoor3 지정. 이 컴포넌트만 슬라이드한다.
	DoorMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("DoorMesh"));
	DoorMesh->SetupAttachment(RootScene);

	// 근접 감지 트리거. 반지름은 BeginPlay 에서 InteractRadius 로 맞춘다.
	InteractSphere = CreateDefaultSubobject<USphereComponent>(TEXT("InteractSphere"));
	InteractSphere->SetupAttachment(RootScene);
	InteractSphere->SetSphereRadius(200.0f);
	InteractSphere->SetCollisionProfileName(TEXT("Trigger")); // 오버랩 전용
	InteractSphere->SetGenerateOverlapEvents(true);

	// 문 위 안내 텍스트.
	PromptText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("PromptText"));
	PromptText->SetupAttachment(RootScene);
	PromptText->SetHorizontalAlignment(EHTA_Center);
	PromptText->SetVerticalAlignment(EVRTA_TextCenter);
	PromptText->SetWorldSize(20.0f);
	PromptText->SetTextRenderColor(FColor::Yellow);
	PromptText->SetRelativeLocation(FVector(0.0f, 0.0f, 120.0f)); // 문 위쪽(BP에서 조정)
	PromptText->SetText(FText::FromString(TEXT("트리거로 열기")));
}

void ADoor::BeginPlay()
{
	Super::BeginPlay();

	// 닫힘 상태의 문 패널 위치 기록(여기서부터 OpenOffset 만큼 슬라이드).
	if (DoorMesh)
	{
		ClosedRelLoc = DoorMesh->GetRelativeLocation();
	}

	// 근접 트리거 설정.
	if (InteractSphere)
	{
		InteractSphere->SetSphereRadius(InteractRadius);
		InteractSphere->OnComponentBeginOverlap.AddDynamic(this, &ADoor::OnRangeBegin);
		InteractSphere->OnComponentEndOverlap.AddDynamic(this, &ADoor::OnRangeEnd);
	}

	// 프롬프트 폰트(한글) + 문구. 시작은 숨김.
	if (PromptText)
	{
		UFont* Font = PromptFont;
		if (!Font)
		{
			static const TCHAR* CandidatePaths[] =
			{
				TEXT("/Game/UI/F_KoreanFont.F_KoreanFont"),
				TEXT("/Game/UI/Fonts/F_KoreanFont.F_KoreanFont"),
				TEXT("/Game/CH/F_KoreanFont.F_KoreanFont"),
			};
			for (const TCHAR* Path : CandidatePaths)
			{
				if (UFont* Found = LoadObject<UFont>(nullptr, Path))
				{
					Font = Found;
					break;
				}
			}
		}
		if (Font)
		{
			PromptText->SetFont(Font);
		}
		// 발광 머티리얼이 지정돼 있으면 글자가 빛나게 적용.
		if (PromptMaterial)
		{
			PromptText->SetTextMaterial(PromptMaterial);
		}
		PromptText->SetText(PromptString);
		PromptText->SetVisibility(false);
	}
}

void ADoor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 목표(열림=1/닫힘=0)로 OpenAlpha 를 보간하며 문 패널을 슬라이드한다.
	const float Target = bIsOpen ? 1.0f : 0.0f;
	if (!FMath::IsNearlyEqual(OpenAlpha, Target, 0.001f))
	{
		const float Step = (OpenDuration > 0.0f) ? (DeltaTime / OpenDuration) : 1.0f;
		OpenAlpha = FMath::Clamp(OpenAlpha + (bIsOpen ? Step : -Step), 0.0f, 1.0f);
		if (DoorMesh)
		{
			DoorMesh->SetRelativeLocation(ClosedRelLoc + OpenOffset * OpenAlpha);
		}
	}
}

void ADoor::OpenDoor()
{
	const bool bWasClosed = !bIsOpen;
	bIsOpen = true;

	// 열리면 프롬프트 숨김.
	if (PromptText)
	{
		PromptText->SetVisibility(false);
	}

	// 닫힘 → 열림 전환일 때만 효과음 1회.
	if (bWasClosed && OpenSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, OpenSound, GetActorLocation());
	}

	// 몇 초 뒤 자동 닫힘(재트리거 시 연장). 0 이하면 자동 닫힘 없음.
	if (AutoCloseDelay > 0.0f)
	{
		GetWorldTimerManager().SetTimer(AutoCloseTimer, this, &ADoor::CloseDoor, AutoCloseDelay, false);
	}
}

void ADoor::CloseDoor()
{
	if (!bIsOpen)
	{
		return;
	}
	bIsOpen = false;

	if (CloseSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, CloseSound, GetActorLocation());
	}

	// 닫히면 범위 안일 때 프롬프트 다시 표시.
	UpdatePrompt();
}

void ADoor::OnRangeBegin(UPrimitiveComponent* /*OverlappedComp*/, AActor* OtherActor, UPrimitiveComponent* /*OtherComp*/, int32 /*OtherBodyIndex*/, bool /*bFromSweep*/, const FHitResult& /*Sweep*/)
{
	// 플레이어 폰만 인정.
	if (OtherActor && OtherActor == UGameplayStatics::GetPlayerPawn(this, 0))
	{
		bPlayerInRange = true;
		UpdatePrompt();
	}
}

void ADoor::OnRangeEnd(UPrimitiveComponent* /*OverlappedComp*/, AActor* OtherActor, UPrimitiveComponent* /*OtherComp*/, int32 /*OtherBodyIndex*/)
{
	if (OtherActor && OtherActor == UGameplayStatics::GetPlayerPawn(this, 0))
	{
		bPlayerInRange = false;
		UpdatePrompt();
	}
}

void ADoor::UpdatePrompt()
{
	// 근접해 있고 아직 닫혀 있을 때만 안내 표시.
	if (PromptText)
	{
		PromptText->SetVisibility(bPlayerInRange && !bIsOpen);
	}
}
