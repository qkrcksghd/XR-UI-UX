// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Door.generated.h"

class UStaticMeshComponent;
class USphereComponent;
class UTextRenderComponent;
class USoundBase;
class UFont;
class UMaterialInterface;
class UPrimitiveComponent;

/**
 * 미닫이(슬라이드) 문.
 *  - 시작은 닫힘. 플레이어가 InteractRadius 안에 들어오면 문 위 3D 텍스트(프롬프트)가 뜬다.
 *  - 플레이어가 왼쪽 트리거(XRPawn 의 InteractAction)를 누르면 OpenDoor() 가 호출돼 OpenOffset 만큼 슬라이드.
 *  - 열린 뒤 AutoCloseDelay 초가 지나면 자동으로 닫힌다.
 *  - 메쉬(SM_MetalDoor3)·열림 오프셋·사운드·프롬프트 문구는 디테일에서 지정.
 */
UCLASS()
class SEAPROJECT_API ADoor : public AActor
{
	GENERATED_BODY()

public:
	ADoor();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// ── 컴포넌트 ─────────────────────────────────────────────
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	USceneComponent* RootScene;

	// 실제 문 패널(SM_MetalDoor3 지정). 이 컴포넌트만 슬라이드한다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	UStaticMeshComponent* DoorMesh;

	// 플레이어 근접 감지(프롬프트/열기 판정). 반지름은 InteractRadius.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	USphereComponent* InteractSphere;

	// 문 위 안내 텍스트("트리거로 열기"). 근접 + 닫힘일 때만 보인다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Door")
	UTextRenderComponent* PromptText;

	// ── 설정 ─────────────────────────────────────────────────
	// 열릴 때 문 패널이 이동할 상대 오프셋(cm). 미닫이 방향/거리. (예: 옆으로 X=200)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	FVector OpenOffset = FVector(200.0f, 0.0f, 0.0f);

	// 여닫는 데 걸리는 시간(초). 작을수록 빠르게 열림.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float OpenDuration = 1.0f;

	// 열린 뒤 자동으로 닫히기까지의 시간(초). 0 이하면 자동 닫힘 없음.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float AutoCloseDelay = 3.0f;

	// 상호작용 가능 거리(프롬프트가 뜨고 트리거로 열 수 있는 범위, cm).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	float InteractRadius = 200.0f;

	// 문 열릴 때 1회 재생할 효과음.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	USoundBase* OpenSound = nullptr;

	// 문 닫힐 때 1회 재생할 효과음(선택).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	USoundBase* CloseSound = nullptr;

	// 프롬프트 문구.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	FText PromptString = FText::FromString(TEXT("트리거로 열기"));

	// (선택) 프롬프트용 한글 폰트. 비우면 약속된 경로(F_KoreanFont)에서 자동 검색.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	UFont* PromptFont = nullptr;

	// (선택) 프롬프트 텍스트 머티리얼. 발광(Emissive) 머티리얼을 넣으면 글자가 빛난다.
	// 비우면 기본 텍스트 머티리얼(안 빛남). TextRender 용 머티리얼이어야 함("Used with Text Render" 체크).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Door")
	UMaterialInterface* PromptMaterial = nullptr;

	// ── 동작 ─────────────────────────────────────────────────
	// 문 열기(슬라이드 시작 + 효과음 + 자동닫힘 타이머). 이미 열려 있으면 타이머만 연장.
	UFUNCTION(BlueprintCallable, Category = "Door")
	void OpenDoor();

	// 문 닫기.
	UFUNCTION(BlueprintCallable, Category = "Door")
	void CloseDoor();

	// 플레이어가 상호작용 범위 안에 있는지 (XRPawn 이 어느 문을 열지 고를 때 사용).
	UFUNCTION(BlueprintPure, Category = "Door")
	bool IsPlayerInRange() const { return bPlayerInRange; }

private:
	UFUNCTION()
	void OnRangeBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& Sweep);

	UFUNCTION()
	void OnRangeEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

	// 근접 + 닫힘일 때만 프롬프트 표시.
	void UpdatePrompt();

	bool bPlayerInRange = false;
	bool bIsOpen = false;                       // 목표 상태(true=열림 지향)
	float OpenAlpha = 0.0f;                     // 0=닫힘, 1=열림 (보간값)
	FVector ClosedRelLoc = FVector::ZeroVector; // 닫힘 상태의 문 패널 상대 위치(BeginPlay에 기록)
	FTimerHandle AutoCloseTimer;
};
