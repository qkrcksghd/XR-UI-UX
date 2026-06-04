// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Engine/EngineTypes.h"
#include "FlashlightScanComponent.generated.h"

class UDataTable;
class USceneComponent;

// 스캔 이벤트 델리게이트 (도감 위젯/물고기 BP 에서 바인딩해서 사용)
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FFlashlightScanStarted, AActor*, Fish, FName, RowName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFlashlightScanProgress, float, Progress01);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FFlashlightScanCompleted, AActor*, Fish, FName, RowName);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFlashlightScanCancelled);

// 손전등 빛 방향으로 트레이스해서, 맞은 물고기를 일정 시간 비추면 스캔이 완료되는 컴포넌트.
// 물고기마다 걸리는 시간은 DT_Encyclopedia 의 컬럼에서 RowName 기준으로 읽어온다.
// 기존 Sonar(BP_Scanner) 시스템과는 완전히 독립적이다.
UCLASS(ClassGroup = (Scan), meta = (BlueprintSpawnableComponent))
class SEAPROJECT_API UFlashlightScanComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UFlashlightScanComponent();

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	// 트레이스 기준이 되는 컴포넌트(손전등/컨트롤러). 보통 소유 폰이 BeginPlay 에서 지정.
	UFUNCTION(BlueprintCallable, Category = "Scan")
	void SetTraceSource(USceneComponent* InSource);

	// 스캔 on/off (예: 트리거를 누르는 동안만 켜고 싶을 때 호출)
	UFUNCTION(BlueprintCallable, Category = "Scan")
	void SetScanningActive(bool bActive);

	// 데이터테이블에서 해당 RowName 의 스캔 시간을 읽어온다. 없으면 DefaultScanDuration.
	UFUNCTION(BlueprintPure, Category = "Scan")
	float GetScanDurationForRow(FName RowName) const;

	// ── 이벤트 ─────────────────────────────────────────────
	UPROPERTY(BlueprintAssignable, Category = "Scan")
	FFlashlightScanStarted OnScanStarted;

	UPROPERTY(BlueprintAssignable, Category = "Scan")
	FFlashlightScanProgress OnScanProgress;

	UPROPERTY(BlueprintAssignable, Category = "Scan")
	FFlashlightScanCompleted OnScanCompleted;

	UPROPERTY(BlueprintAssignable, Category = "Scan")
	FFlashlightScanCancelled OnScanCancelled;

	// ── 도감 데이터 설정 ────────────────────────────────────
	// DT_Encyclopedia 를 에디터에서 지정.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scan|Data")
	UDataTable* EncyclopediaTable = nullptr;

	// 스캔 시간이 들어있는 컬럼(변수) 이름. DT 의 실제 컬럼명과 맞춰줄 것.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scan|Data")
	FName ScanDurationColumn = FName("ScanDuration");

	// 행/컬럼을 못 찾았을 때 사용할 기본 스캔 시간(초).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scan|Data", meta = (ClampMin = "0.0"))
	float DefaultScanDuration = 3.0f;

	// ── 트레이스 설정 ──────────────────────────────────────
	// 빛이 닿는 최대 거리.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scan|Trace", meta = (ClampMin = "0.0"))
	float ScanRange = 2000.0f;

	// 트레이스 두께(0 이면 가는 라인, >0 이면 그만큼 퍼진 구체 스윕 → 조준이 덜 빡빡함).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scan|Trace", meta = (ClampMin = "0.0"))
	float ScanRadius = 20.0f;

	// 트레이스에 사용할 채널. 물고기가 이 채널을 Block 해야 인식됨.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scan|Trace")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_Visibility;

	// 빛이 물고기를 벗어나면 진행도를 0으로 리셋할지(false 면 멈춘 채 유지).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scan|Trace")
	bool bResetProgressOnLoseTarget = true;

	// 디버그 라인 표시.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scan|Trace")
	bool bDrawDebug = false;

	// 컴포넌트 활성화 여부.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Scan")
	bool bScanningActive = true;

	// ── 런타임 상태(읽기 전용) ──────────────────────────────
	// 지금 스캔 진행 중인지. 도감의 isScanning 을 여기에 맞추면 됨.
	UPROPERTY(BlueprintReadOnly, Category = "Scan|State")
	bool bIsScanning = false;

	UPROPERTY(BlueprintReadOnly, Category = "Scan|State")
	float CurrentProgress = 0.0f;

	UPROPERTY(BlueprintReadOnly, Category = "Scan|State")
	AActor* CurrentTarget = nullptr;

private:
	// 현재 대상의 스캔을 취소/초기화.
	void CancelCurrent();

	UPROPERTY()
	USceneComponent* TraceSource = nullptr;

	float Accum = 0.0f;            // 누적 비춘 시간
	float CurrentDuration = 0.0f;  // 현재 대상의 목표 시간
	FName CurrentRowName = NAME_None;
	bool bCompletedForTarget = false; // 현재 대상 스캔 완료 처리됨(중복 방지)
};
