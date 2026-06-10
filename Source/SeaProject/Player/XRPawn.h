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
class UFont;
class USkeletalMesh;

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

	// 스캔 중 락온된 물고기가 손전등에서 이 거리(cm) 이상 멀어지면 스캔 자동 취소("소나 범위" 이탈).
	// 물고기가 도망쳐 이 범위를 벗어나면 스캔이 끊긴다. 0 이하면 거리 제한 없음(끝까지 락온).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Scan")
	float ScanCancelRange = 2000.0f;

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

	// 스캔 중인 물고기 메쉬를 감싸는 하이라이트(오버레이) 머티리얼. 스캔 시작 시 물고기 메쉬에 입히고
	// 끝나면 벗긴다 → 지금 어떤 물고기를 스캔 중인지 한눈에 보인다. 비우면 효과 없음.
	// (프레넬 글로우/스캔라인 같은 머티리얼을 만들어 디테일에서 지정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Scan")
	class UMaterialInterface* ScanOverlayMaterial = nullptr;

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

	// 상호작용 입력(문 열기 등). IMC_VR 에서 이 액션에 왼쪽 트리거를 매핑한다.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "XR | Input")
	UInputAction* InteractAction;

	// ── 수심 표시 ────────────────────────────────────────────────────
	// 수심 0(해수면)에 해당하는 Z. FishSpawner 의 SurfaceZ 와 같은 기준(0)으로 맞춤.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Depth")
	float WaterSurfaceZ = 0.0f;

	// 1미터당 언리얼 Z 유닛 수. FishSpawner 의 UnitsPerMeter(3.175)와 동일하게 맞춤.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Depth")
	float DepthUnitsPerMeter = 3.175f;

	// 수심 디버그: 켜면 화면에 현재 폰 Z·수면 Z·계산된 수심을 표시한다. WaterSurfaceZ 보정에 쓴다(평소엔 끔).
	// → 수면 위치에 섰을 때 표시되는 PawnZ 값을 WaterSurfaceZ 에 넣으면 그 지점이 "수심 0m" 이 된다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Depth")
	bool bShowDepthDebug = false;

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

	// ── 도감 텍스트 (손목 시계) ──────────────────────────────────────
	// 홀로그램(도감)이 열리면 위 DepthText 대신 아래 세 텍스트가 켜진다(C++ 가 자동 토글).
	// 셋 다 이 루트 밑에 묶여 있으니, BP 에서 루트만 시계 화면 위치/각도에 맞춰 옮기면 된다.
	// (홀로그램 메쉬와 분리되어 있어, 메쉬는 BP_HologramDisplay 쪽에서 원하는 곳에 따로 띄우면 됨)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Encyclopedia")
	USceneComponent* EncyclopediaTextRoot;

	// 종 이름 (DT 의 Name)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Encyclopedia")
	UTextRenderComponent* EncyNameText;

	// "스테이지 N" (DT 의 StageLevel)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Encyclopedia")
	UTextRenderComponent* EncyStageText;

	// "깊이 Nm" (DT 의 서식 수심 — 플레이어 현재 수심 아님)
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Encyclopedia")
	UTextRenderComponent* EncyDepthText;

	// (선택) 한글 폰트. 비우면 약속된 경로(/Game/UI/F_KoreanFont 등)에서 자동으로 찾는다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Encyclopedia")
	UFont* EncyclopediaFont = nullptr;

	// 이름 텍스트 자동 크기 맞춤(긴 이름이 시계 화면을 넘지 않게).
	//  - MaxWidth: 이름이 차지할 수 있는 최대 가로 폭(cm, 로컬). 시계 화면 폭에 맞춰 조정.
	//  - MaxSize : 짧은 이름일 때 쓰는 크기(= 기본/최대 글자 크기). 이 값으로 EncyNameText 의 World Size 를 매번 덮어쓴다.
	//  - MinSize : 아주 긴 이름이라도 이보다 작아지진 않음.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Encyclopedia")
	float EncyNameMaxWidth = 12.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Encyclopedia")
	float EncyNameMaxSize = 8.0f;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Encyclopedia")
	float EncyNameMinSize = 3.0f;

	// 스테이지/깊이 텍스트 글자 크기(이름과 별개로 여기서 한 번에 관리). 이름 크기는 EncyNameMaxSize.
	// C++ 가 BeginPlay 에 세 텍스트의 Scale=1 + 이 크기들로 강제 → BP 오버라이드로 제각각 커지는 것 방지.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Encyclopedia")
	float EncyInfoSize = 6.0f;

	// ── 홀로그램 액터 (C++ 가 전담: 스폰/부착/숨김/메쉬교체. BP 로직 불필요) ──
	// 폰 디테일에서 BP_HologramDisplay 클래스를 지정한다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Encyclopedia")
	TSubclassOf<AActor> HologramClass;

	// 홀로그램을 왼손 기준으로 놓을 상대 트랜스폼(위치+회전+스케일). 시계 위에 맞게 디테일에서 조정.
	// (스폰 후 손에 붙인 다음 이 트랜스폼을 그대로 적용한다 → 회전까지 마음대로)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Encyclopedia")
	FTransform HologramLocalTransform;

	// 아직 "안 잡은" 물고기에 입힐 홀로그램(유령) 머티리얼. 수집한 물고기는 원래 머티리얼로 보인다.
	// 폰 디테일에서 지정. 비우면 항상 원래 머티리얼로 표시.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Encyclopedia")
	class UMaterialInterface* HologramMaterial = nullptr;

	// 런타임에 스폰된 홀로그램 인스턴스 (읽기 전용).
	UPROPERTY(BlueprintReadOnly, Category = "XR | Encyclopedia")
	AActor* HologramInstance = nullptr;

	// 도감이 열려 있을 때 홀로그램 물고기 모델을 돌리는 속도(초당 도, Pitch/Yaw/Roll).
	// 기본 Yaw 45°/s(수직축 턴테이블 회전). 0,0,0 이면 회전 안 함. 도는 축이 이상하면 디테일에서 조정.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Encyclopedia")
	FRotator HologramSpinRate = FRotator(0.0f, 45.0f, 0.0f);

	// 현재 보고 있는 도감 행 이름(EncyclopediaTable RowName). 홀로그램 메쉬/모델을 이 행에 맞춰
	// 갱신하고 싶을 때 BP 에서 읽으면 된다(예: GetRowModelMesh 입력).
	UFUNCTION(BlueprintPure, Category = "Encyclopedia")
	FName GetCurrentEncyclopediaRowName() const;

	// ── 보물상자 (히든 퀘스트) ────────────────────────────────────────
	// 별도 BP 불필요 — 레벨의 Chest_of_Gold 액터에 "Chest" 태그만 추가하면 손전등 스캔 대상이 된다.
	// 물고기와 같은 스캔 흐름(조준+트리거 홀드+진행도)을 타고, 완료되면 아래 설정으로 처리한다.

	// 상자 수집 완료 순간 상자 위치에 1회 스폰할 나이아가라 이펙트. 폰 디테일에서 지정.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Chest")
	class UNiagaraSystem* ChestCollectEffect = nullptr;

	// 상자 수집 완료 시 1회 재생할 사운드(선택).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Chest")
	USoundBase* ChestCollectSound = nullptr;

	// 상자 스캔에 걸리는 시간(초). 물고기와 별개로 상자는 이 값을 쓴다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Chest")
	float ChestScanDuration = 2.0f;

	// 상자를 하나 수집한 순간 1회 호출 → BP에서 "1/4" 팝업을 잠깐 띄운다.
	// Collected = 지금까지 수집한 상자 수, Total = 레벨의 전체 상자 수.
	UFUNCTION(BlueprintImplementableEvent, Category = "XR | Chest")
	void OnChestCollected(int32 Collected, int32 Total);

	// 레벨의 전체 상자 수("Chest" 태그 액터 수). 1/4 의 분모.
	UFUNCTION(BlueprintPure, Category = "XR | Chest")
	int32 GetChestTotal() const;

	// 지금까지 수집한 상자 수. 1/4 의 분자.
	UFUNCTION(BlueprintPure, Category = "XR | Chest")
	int32 GetChestCollectedCount() const;

	// 카메라 앞에 잠깐 뜨는 "1/4" 수집 카운트 3D 텍스트. C++ 가 수집할 때 자동으로 켜고(몇 초 뒤) 끈다.
	// 카메라에 붙어 시야를 따라다닌다. 위치/크기/각도는 BP 에서 이 컴포넌트를 옮겨 조정.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "XR | Chest")
	UTextRenderComponent* ChestCountText;

	// "1/4" 팝업이 화면에 떠 있는 시간(초). 이 시간이 지나면 자동으로 숨긴다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Chest")
	float ChestCountShowSeconds = 3.0f;

	// ── 엔딩 연출 (도감 완성) ──────────────────────────────────────────
	// 도감을 다 채우면(수집 종 수 == 전체 종 수) 조작이 전부 잠기고, 플레이어가 수면까지
	// 천천히 떠올라 화면이 페이드 아웃되는 시네마틱이 자동 재생된다.

	// 도감 "완성"으로 칠 수집 종 수. 0이면 EncyclopediaTable 의 전체 행 수를 자동 사용.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Ending")
	int32 EncyclopediaCompleteCount = 0;

	// 수면까지 떠오르는 데 걸리는 시간(초). 깊이와 무관하게 이 시간에 맞춰 이즈인아웃으로 상승.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Ending")
	float AscentDuration = 8.0f;

	// 상승 중 지형/벽에 안 걸리도록 캡슐 콜리전을 끌지.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Ending")
	bool bDisableCollisionDuringAscent = true;

	// 수면 도착 후 화면 페이드 시간(초).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Ending")
	float EndingFadeDuration = 2.5f;

	// 페이드 색(기본 흰색 — 수면 위 햇빛으로 떠오르는 느낌). 검은색으로 바꾸면 암전.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Ending")
	FLinearColor EndingFadeColor = FLinearColor::White;

	// 페이드 아웃이 "끝난 뒤" 메인메뉴로 넘어가기까지 추가로 대기하는 시간(초).
	// (페이드된 화면을 잠깐 보여준 뒤 전환 — 너무 빨리 넘어가지 않게)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Ending")
	float EndingHoldBeforeMenu = 5.0f;

	// 페이드 아웃이 끝나면 돌아갈 레벨(메인메뉴). 짧은 이름("MainMenu") 또는 전체 경로.
	// 비우면(None) 레벨 전환 없이 페이드된 채로 멈춘다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "XR | Ending")
	FName MainMenuLevelName = TEXT("MainMenu");

	// 도감이 완성됐는지(수집 종 수 ≥ 필요 종 수).
	UFUNCTION(BlueprintPure, Category = "XR | Ending")
	bool IsEncyclopediaComplete() const;

	// 엔딩 시네마틱 시작(조작 전부 잠금 + 수면까지 상승). 보통 도감 완성 시 C++ 가 자동 호출.
	// BP에서 직접 호출하면 테스트/강제 트리거도 가능.
	UFUNCTION(BlueprintCallable, Category = "XR | Ending")
	void StartEndingSequence();

	// 엔딩 시작 순간 1회 호출 → BP에서 음악/연출(선택).
	UFUNCTION(BlueprintImplementableEvent, Category = "XR | Ending")
	void OnEndingStarted();

	// 수면 도착(상승 완료) 순간 1회 호출 → BP에서 크레딧/메뉴 등(선택). 화면 페이드는 C++ 가 이미 처리.
	UFUNCTION(BlueprintImplementableEvent, Category = "XR | Ending")
	void OnReachedSurface();

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
	// 오른손 B 버튼을 누르면 호출 → 블루프린트에서 홀로그램 표시/숨김을 구현한다.
	// bOpen: 이번 토글 "후" 상태(true=열림). 손목 수심텍스트(DepthText)는 C++ 가 자동으로 반대로 켜고 끈다.
	//   - true  → BP_HologramDisplay 스폰/표시 + AttachHologramToHand + 데이터테이블 행 표시
	//   - false → 홀로그램 숨김/제거(DetachHologram)
	UFUNCTION(BlueprintImplementableEvent, Category = "Encyclopedia")
	void OnToggleEncyclopedia(bool bOpen);

	// 도감(홀로그램)이 현재 열려 있는지. 열려 있으면 손목 수심텍스트는 꺼져 있다.
	UFUNCTION(BlueprintPure, Category = "Encyclopedia")
	bool IsEncyclopediaOpen() const { return bEncyclopediaOpen; }

	// 오른손 썸스틱 X를 한 번 꺾을 때마다 호출 → 블루프린트에서 페이지 넘김을 구현한다.
	// Direction: +1 = 다음 페이지(오른쪽), -1 = 이전 페이지(왼쪽).
	// (이제 메쉬 갱신은 아래 OnEncyclopediaRowChanged 로 처리하면 되니, 이건 효과음 등에만 써도 됨)
	UFUNCTION(BlueprintImplementableEvent, Category = "Encyclopedia")
	void OnEncyclopediaPage(int32 Direction);

	// 표시 중인 도감 행이 바뀔 때(열 때 + 페이지 넘길 때) C++ 가 호출한다.
	// RowName  = 현재 행 이름(이름 텍스트와 동일한 행 — 항상 일치 보장),
	// ModelMesh = 그 행의 ModelMesh(없으면 null).
	// → BP 에서 홀로그램 SkeletalMesh 를 ModelMesh 로 세팅하면 끝. (BP 자체 페이지 인덱스 로직은 전부 삭제 가능)
	UFUNCTION(BlueprintImplementableEvent, Category = "Encyclopedia")
	void OnEncyclopediaRowChanged(FName RowName, USkeletalMesh* ModelMesh);

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
	void AttachHologramToHand(AActor* Hologram, bool bRightHand, const FTransform& LocalTransform);

	// 손에서 떼어내 현재 위치에 그대로 둔다.
	UFUNCTION(BlueprintCallable, Category = "XR | Hologram")
	void DetachHologram(AActor* Hologram);

private:
	// ── 입력 처리 함수 (Binding) ──────────────────────────────────
	void Input_Move(const FInputActionValue& Value);


	// BP_Scanner 소환 (Y 버튼)
	void Input_SpawnSonar();

	// 상호작용 (왼쪽 트리거) — 범위 안의 가장 가까운 문(ADoor)을 연다.
	void Input_Interact();

	// 손전등 스캔(물고기 수집) — 매 틱 호출. 손전등이 켜져 있을 때만 진행도 누적.
	void UpdateFlashlightScan(float DeltaTime);

	// 물고기의 헤엄 컴포넌트에 도망 모드 ON/OFF. ON 이면 플레이어(이 폰) 위치에서 멀어지게 빠르게 도망.
	void SetFishFleeing(AActor* Fish, bool bFlee);

	// 스캔 하이라이트 ON/OFF. 물고기의 모든 메쉬 컴포넌트에 ScanOverlayMaterial 을 입히거나(ON) 벗긴다(OFF).
	void SetScanHighlight(AActor* Fish, bool bOn);

	// 이 상자 액터가 이미 수집됐는지(서브시스템 조회). 상자 키는 액터 고유 이름.
	bool IsChestCollected(AActor* Chest) const;

	// "1/4" 카운트 텍스트를 켜고(내용 갱신) ChestCountShowSeconds 뒤에 숨긴다.
	void ShowChestCount(int32 Collected, int32 Total);
	void HideChestCount();
	FTimerHandle ChestCountHideTimer;

	// 엔딩 연출 상태(상승 진행).
	bool bEndingActive = false;
	bool bEndingReachedSurface = false;
	float EndingElapsed = 0.0f;
	float EndingStartZ = 0.0f;

	// 페이드 종료 후 메인메뉴로 전환(타이머 콜백).
	void ReturnToMainMenu();
	FTimerHandle EndingMenuTimer;

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

	// 도감(홀로그램) 열림 상태. 열리면 손목 수심텍스트(DepthText)를 끄고, 닫히면 다시 켠다(둘 중 하나만 표시).
	bool bEncyclopediaOpen = false;

	// 홀로그램 안의 물고기 모델(스켈레탈 메쉬) 컴포넌트 캐시. 스폰 시 자동으로 찾아둔다.
	UPROPERTY()
	class USkeletalMeshComponent* HologramFishMesh = nullptr;

	// 현재 도감 페이지 = EncyclopediaTable 행 인덱스.
	int32 EncyclopediaRowIndex = 0;

	// 현재 행 인덱스로 세 텍스트(Name/Stage/Depth)를 다시 채운다.
	void RefreshEncyclopediaText();

	// 페이지 이동(+1=다음/-1=이전). 인덱스를 옮기고 텍스트를 갱신한 뒤 BP(메쉬 갱신용)에 알린다.
	// 도감이 닫혀 있으면 아무것도 안 한다.
	void AdvanceEncyclopediaPage(int32 Direction);

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
