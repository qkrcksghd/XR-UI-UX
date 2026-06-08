// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h" // Enhanced Input 필수 헤더
#include "XRPawn.generated.h"

// 컴포넌트 전방 선언 (빌드 속도 향상)
class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class USphereComponent;
class UCapsuleComponent;
class USkeletalMeshComponent;
class UInputMappingContext;
class UInputAction;
class UFloatingPawnMovement;
class USpotLightComponent;
class USoundBase;
class UAudioComponent;
class ABackgroundMusic;
class UTextRenderComponent;

UCLASS()
class SEAPROJECT_API AXRPawn : public APawn
{
	GENERATED_BODY()

public:
	AXRPawn();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;

	// 입력 컴포넌트 설정 오버라이드
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ── 계층 구조 컴포넌트 ───────────────────────────────────────────
	// 충돌 루트(캡슐) — 폰이 벽/바닥/BlockingVolume에 막히게 하는 콜리전. 폰의 진짜 루트.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Components")
	UCapsuleComponent* CapsuleRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Components")
	USceneComponent* VRRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Components")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Components")
	UMotionControllerComponent* LeftHandController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Components")
	UMotionControllerComponent* RightHandController;

	// 그랩 범위를 시각화하거나 로직에 사용할 충돌 구체
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Components")
	USphereComponent* LeftGrabSphere;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Components")
	USphereComponent* RightGrabSphere;

	// ── 손전등 스캔 ───────────────────────────────────────────────────
	// 오른손 컨트롤러에 붙는 손전등(빛 연출 + 트레이스 방향 기준)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Scan")
	USpotLightComponent* Flashlight;

	// 소나(BP_Scanner) 클래스 — 폰 디테일에서 BP_Scanner 로 지정한다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XR | Scan")
	TSubclassOf<AActor> SonarClass;

	// 런타임에 스폰된 소나 인스턴스 (왼손 그립 시 DoScan 호출 대상)
	UPROPERTY(BlueprintReadOnly, Category = "XR | Scan")
	AActor* SonarInstance;

	// ── 손전등 스캔(물고기 수집) ──────────────────────────────────────
	// 손전등을 물고기에 이만큼(초) 계속 비추면 수집 완료
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Scan")
	float ScanDuration = 2.0f;

	// (선택) 도감 데이터테이블(DT_Encyclopedia). 지정하면 스캔 완료 시 물고기를 정확한 행 이름으로
	// 매칭해 수집 등록한다(만다린 오타 등도 보정). 비워두면 물고기 클래스 이름으로 등록.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Scan")
	class UDataTable* EncyclopediaTable;

	// 손전등 스캔 트레이스 최대 거리(cm)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Scan")
	float ScanRange = 1500.0f;

	// 손전등/스캔 조준 보정(회전). 컨트롤러 +X 와 손에 든 장치가 "가리키는 방향"이
	// 어긋날 때, 이 값으로 빛+스캔 콘을 장치 정면에 맞춘다.
	// PIE에서 디버그 콘을 보며 Yaw/Pitch 를 조금씩 돌려 맞추면 된다. (BeginPlay에서 적용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Scan")
	FRotator FlashlightAimOffset = FRotator::ZeroRotator;

	// 스캔 원뿔: 중심에서 좌우로 벌어지는 반각(도). 좁을수록 정밀 조준.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Scan")
	float ScanConeHalfAngle = 8.0f;

	// [미사용] 예전 라인트레이스 방식의 부채꼴 가닥 수. 현재는 각도/거리 판정이라 사용 안 함.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Scan")
	int32 ScanRayCount = 9;

	// 스캔 디버그 표시(콘/중심선/화면 메시지). 개발 중 조준 맞출 때만 켠다. 평소엔 false.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Scan")
	bool bShowScanDebug = false;

	// 현재 스캔 진행도(초) — 읽기 전용
	UPROPERTY(BlueprintReadOnly, Category = "XR | Scan")
	float ScanProgress = 0.0f;

	// 현재 스캔 중인 물고기 — 읽기 전용
	UPROPERTY(BlueprintReadOnly, Category = "XR | Scan")
	AActor* CurrentScanTarget;

	// ── 스캔 사운드 ───────────────────────────────────────────────────
	// 스캔하는 동안 반복 재생되는 사운드(루프). 펀 디테일에 에셋만 지정하면 됨. 비우면 무음.
	// (사운드 에셋의 Looping 을 안 켜도, 아래 오디오 컴포넌트가 루프로 재생한다.)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Scan")
	USoundBase* ScanLoopSound;

	// 스캔 완료 순간 1회 재생되는 사운드. 펀 디테일에 에셋만 지정하면 됨. 비우면 무음.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Scan")
	USoundBase* ScanCompleteSound;

	// 루프 사운드 재생용 오디오 컴포넌트(C++ 가 자동 생성/제어). 직접 만질 필요 없음.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Scan")
	UAudioComponent* ScanLoopAudio;

	// 소나(BP_Sonar)를 "가동"할 때(Y버튼) 1회 재생되는 사운드. C++ 가 자동 재생. 비우면 무음.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Scan")
	USoundBase* SonarActivateSound;

	// 소나가 "실제로 물고기를 감지한 순간" 재생할 사운드. BP_Sonar 에서 PlaySonarDetectSound 노드로 재생. 비우면 무음.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Scan")
	USoundBase* SonarDetectSound;

	// 소나 "가동음"(SonarActivateSound) 1회 재생. C++ 가 Y버튼 시 자동 호출.
	UFUNCTION(BlueprintCallable, Category = "XR | Scan")
	void PlaySonarActivateSound();

	// 소나 "감지음"(SonarDetectSound) 1회 재생. BP_Sonar 가 물고기를 감지한 순간 이 노드를 호출하면 된다.
	UFUNCTION(BlueprintCallable, Category = "XR | Scan")
	void PlaySonarDetectSound();

	// 현재 대상에 적용 중인 실제 스캔 시간(초) — 읽기 전용.
	// 대상이 바뀔 때 EncyclopediaTable 의 그 물고기 행 ScanTime 값으로 세팅한다.
	// 테이블/행/ScanTime 이 없으면 ScanDuration(기본값)으로 폴백한다.
	UPROPERTY(BlueprintReadOnly, Category = "XR | Scan")
	float CurrentScanDuration = 2.0f;

	// 현재 스캔 진행도(0~1) — 읽기 전용. 물고기별 ScanTime 을 기준으로 계산되며,
	// 시간이 다 차면 1.0(=100%) 이 된다. 화면 UI(프로그레스 바/퍼센트)는 이 값에 바로 바인딩하면
	// 물고기마다 알아서 다른 속도로 0%→100% 가 채워진다. (ScanProgress/ScanDuration 으로 직접 계산하지 말 것)
	UPROPERTY(BlueprintReadOnly, Category = "XR | Scan")
	float ScanPercent = 0.0f;

	// 손전등이 향하는 물고기 반환 (태그 "Fish" 가진 액터, 없으면 null)
	UFUNCTION(BlueprintCallable, Category = "XR | Scan")
	AActor* GetFlashlightScanTarget();

	// 스캔이 "새로 시작"될 때 1회 호출(어떤 물고기를 처음 비추기 시작/다른 물고기로 옮긴 순간).
	// → BP에서 "스캔 중 루프 사운드"를 재생(시작)하는 데 쓴다. 정지는 OnScanReset/OnFishScanned 에서.
	// ExpectedDuration: 이 물고기의 스캔 완료까지 걸리는 시간(초) — 사운드 길이/피치 맞추는 데 활용 가능.
	UFUNCTION(BlueprintImplementableEvent, Category = "XR | Scan")
	void OnScanStarted(AActor* Fish, float ExpectedDuration);

	// 스캔 진행 중 매 프레임 호출 → BP에서 진행도 UI(0~1) 표시
	UFUNCTION(BlueprintImplementableEvent, Category = "XR | Scan")
	void OnScanProgress(AActor* Fish, float Percent);

	// 스캔 완료 시 1회 호출 → BP에서 도감 IsCollected 처리 + 갱신
	UFUNCTION(BlueprintImplementableEvent, Category = "XR | Scan")
	void OnFishScanned(AActor* Fish);

	// 스캔이 "완료되지 않고" 중단됐을 때 1회 호출(물고기를 콘 밖으로 놓침 / 트리거 뗌).
	// → BP에서 진행중 UI(스피너 등)를 숨기는 데 쓴다. (완료 때는 호출 안 됨 → OnFishScanned 사용)
	UFUNCTION(BlueprintImplementableEvent, Category = "XR | Scan")
	void OnScanReset();

	// 소나(BP_Sonar)가 새로 소환된 직후 1회 호출 → BP에서 위젯에 ScannerRef 연결 등
	UFUNCTION(BlueprintImplementableEvent, Category = "XR | Scan")
	void OnSonarSpawned(AActor* Sonar);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Components")
	class UFloatingPawnMovement* MovementComp;
	// ── 향상된 입력 (Enhanced Input) ────────────────────────────────
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XR | Input")
	UInputMappingContext* VRMappingContext;

	// 이동 입력 (Vector2D: 조이스틱 X, Y)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XR | Input")
	UInputAction* MoveAction;

	// 그랩 입력 (Digital: 버튼 누름/뗌)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XR | Input")
	UInputAction* GrabLeftAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XR | Input")
	UInputAction* GrabRightAction;

	// 대시 입력. IMC_VR 에서 이 액션에 원하는 버튼들(트리거/그립/A/B 등)을
	// 전부 매핑하면 "아무 버튼이나 누르면 대시"가 된다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XR | Input")
	UInputAction* DashAction;

	// 손전등 온/오프 토글 입력. IMC_VR 에서 이 액션에 VR X 버튼을 매핑한다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XR | Input")
	UInputAction* FlashlightAction;

	// 홀로그램 도감 열기/닫기 입력. IMC_VR 에서 오른손 B 버튼을 매핑한다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XR | Input")
	UInputAction* EncyclopediaToggleAction;

	// 홀로그램 도감 페이지 넘김 입력. IMC_VR 에서 오른손 썸스틱 X축을 매핑한다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XR | Input")
	UInputAction* EncyclopediaPageAction;

	// BP_Scanner 소환 입력. IMC_VR 에서 이 액션에 Y 버튼을 매핑한다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XR | Input")
	UInputAction* SpawnSonarAction;

	// ── 수심 표시 ────────────────────────────────────────────────────
	// 수심 0(해수면)에 해당하는 Z. FishSpawner 의 SurfaceZ 와 같은 기준(0)으로 맞춤.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Depth")
	float WaterSurfaceZ = 0.0f;

	// 1미터당 언리얼 Z 유닛 수. FishSpawner 의 UnitsPerMeter(3.175)와 동일하게 맞춤.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Depth")
	float DepthUnitsPerMeter = 3.175f;

	// 현재 플레이어 수심(미터). 수면 위면 0. 화면 위젯 텍스트에 이 값을 바인딩하면 됨.
	UFUNCTION(BlueprintPure, Category = "XR | Depth")
	float GetPlayerDepthMeters() const;

	// "Depth 12m" 형식 텍스트. UMG Text 에 바로 바인딩하기 좋음.
	UFUNCTION(BlueprintPure, Category = "XR | Depth")
	FText GetPlayerDepthText() const;

	// 손 화면(메쉬) 위에 띄우는 수심 3D 텍스트. C++ 가 매 틱 자동 갱신.
	// BP 에서 이 컴포넌트를 화면 메쉬 위치/각도에 맞게 옮기기만 하면 된다.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Depth")
	UTextRenderComponent* DepthText;

	// ── 이동 속도 설정 ────────────────────────────────────────────────
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Movement")
	float MoveSpeed = 300.0f;

	// 대시(부스트) 중 속도
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Movement")
	float DashSpeed = 900.0f;

	// 현재 대시 중인지 (읽기 전용)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Movement")
	bool bIsDashing = false;

	// ── 홀로그램 도감 (구현은 블루프린트에서) ─────────────────────────
	// 오른손 B 버튼을 누르면 호출 → 블루프린트에서 도감 열기/닫기 토글을 구현한다.
	UFUNCTION(BlueprintImplementableEvent, Category = "Encyclopedia")
	void OnToggleEncyclopedia();

	// 오른손 썸스틱 X를 한 번 꺾을 때마다 호출 → 블루프린트에서 페이지 넘김을 구현한다.
	// Direction: +1 = 다음 페이지(오른쪽), -1 = 이전 페이지(왼쪽).
	UFUNCTION(BlueprintImplementableEvent, Category = "Encyclopedia")
	void OnEncyclopediaPage(int32 Direction);

	// 조이스틱 축을 "한 번 꺾을 때 1회"로 만드는 디바운스 헬퍼.
	// 매 프레임(Triggered) AxisValue 를 넣어 호출하면, 스틱을 FireThreshold 이상 꺾는 "순간"에만
	// +1(오른쪽)/-1(왼쪽)을 1회 돌려주고, 그 외엔 0을 돌려준다. 스틱이 ResetThreshold 아래로
	// 돌아와야 다시 1회 발동한다. (BP에서 페이지 넘김 노드 앞에 끼워 연속 넘김을 막는 용도)
	UFUNCTION(BlueprintCallable, Category = "Encyclopedia", meta = (AdvancedDisplay = "FireThreshold,ResetThreshold"))
	int32 ConsumeAxisFlick(float AxisValue, float FireThreshold = 0.7f, float ResetThreshold = 0.3f);

	// ── 홀로그램 손 따라다니기 ─────────────────────────────────────────
	// 홀로그램(아무 액터)을 손 컨트롤러에 붙여서 손을 따라 계속 움직이게 한다(잡기 불필요).
	// bRightHand=false 면 왼손에 붙음(오른손은 스캔용). LocalOffset = 손에서 떨어뜨릴 위치(cm).
	// 도감 띄울 때 1번 호출 → 닫을 때 DetachHologram 호출.
	UFUNCTION(BlueprintCallable, Category = "XR | Hologram")
	void AttachHologramToHand(AActor* Hologram, bool bRightHand, FVector LocalOffset);

	// 손에서 떼어내 현재 위치에 그대로 둔다.
	UFUNCTION(BlueprintCallable, Category = "XR | Hologram")
	void DetachHologram(AActor* Hologram);

private:
	// ── 입력 처리 함수 (Binding) ──────────────────────────────────
	void Input_Move(const FInputActionValue& Value);


	// BP_Scanner 소환 (Y 버튼)
	void Input_SpawnSonar();

	// 손전등 스캔(물고기 수집) — 매 틱 호출. 손전등이 켜져 있을 때만 진행도 누적.
	void UpdateFlashlightScan(float DeltaTime);

	// 물고기의 헤엄 컴포넌트에 도망 모드 ON/OFF. ON 이면 플레이어(이 폰) 위치에서 멀어지게 빠르게 도망.
	void SetFishFleeing(AActor* Fish, bool bFlee);

	// 스캔 중 루프 사운드 재생/정지.
	void SetScanLoopSound(bool bOn);

	// 스캔 루프 사운드가 한 번 끝났을 때 호출(에셋이 루프가 아니어도 스캔 중이면 다시 재생해 끊기지 않게 함).
	UFUNCTION()
	void OnScanLoopFinished();

	// 배경음악 일시정지/재개 (스캔 중엔 멈췄다가 끝나면 이어 재생). 레벨의 ABackgroundMusic 을 찾아 제어.
	void SetBackgroundMusicPaused(bool bPaused);

	// 찾아둔 배경음악 액터 캐시 (매번 검색하지 않게).
	UPROPERTY()
	ABackgroundMusic* CachedBGM = nullptr;

	// 소나음 재생 시 배경음을 끄고, 그 사운드 길이만큼 뒤에 다시 켠다(이어듣기). Sound 가 null 이면 아무것도 안 함.
	void DuckBackgroundMusicForSound(USoundBase* Sound);

	// 소나음이 끝나는 시점에 배경음을 다시 켜는 콜백(스캔 중이면 켜지 않음).
	void ResumeBackgroundMusicAfterSonar();

	// 소나음 종료 후 배경음 재개용 타이머.
	FTimerHandle SonarBGMResumeTimer;

	// 수심 텍스트가 마지막으로 표시한 정수 미터값 (바뀔 때만 SetText 하려고 캐시).
	int32 LastShownDepthMeters = -100000;

	// 손전등 온/오프 토글 (X 버튼)
	void ToggleFlashlight();

	// 손전등 현재 켜짐 상태 (토글용)
	bool bFlashlightOn = true;

	// 스캔 시작/종료 (오른손 트리거를 꾹 누르는 동안만 스캔) — Hold 방식
	// Started=누름 → 스캔 ON, Completed/Canceled=뗌 → 스캔 OFF
	void StartScan();
	void StopScan();

	// 스캔 모드 현재 상태 (트리거를 누르고 있는 동안 true)
	bool bScanActive = false;

	// 도감 열기/닫기 (오른손 B)
	void Input_EncyclopediaToggle();

	// 도감 페이지 넘김 (오른손 썸스틱 X) — 한 번 꺾을 때 1페이지만 발동되도록 디바운스
	void Input_EncyclopediaPage(const FInputActionValue& Value);
	void Input_EncyclopediaPageReleased();

	// 스틱을 중앙으로 되돌리기 전엔 페이지 넘김을 1회만 발동시키기 위한 플래그
	bool bPageFlickEngaged = false;

	// ConsumeAxisFlick 이 마지막으로 호출된 프레임. Enhanced Input "Triggered" 는 스틱이
	// 중앙일 때 호출이 끊기므로, 호출이 끊겼다 다시 들어온(프레임 간격이 벌어진) 것을 보고
	// "스틱을 놨다 다시 꺾었다"고 판단해 플래그를 리셋한다.
	uint64 LastFlickFrame = 0;

	// 오른쪽 그랩
	void Input_GrabRight_Started();
	void Input_GrabRight_Completed();

	// 대시 시작/종료 (버튼 누름/뗌)
	void StartDash();
	void StopDash();

	// ── 그랩 로직 함수 ──────────────────────────────────────────────
	void AttemptGrab(USphereComponent* GrabSphere, UMotionControllerComponent* TargetController);
	void ReleaseGrab(UMotionControllerComponent* TargetController);

	// 현재 잡고 있는 액터를 저장할 변수 (간단한 구현용)
	UPROPERTY()
	AActor* HeldActorLeft;

	UPROPERTY()
	AActor* HeldActorRight;
};
