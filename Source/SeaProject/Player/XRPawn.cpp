// Fill out your copyright notice in the Description page of Project Settings.

#include "XRPawn.h"
#include "Components/SceneComponent.h"
#include "Camera/CameraComponent.h"
#include "MotionControllerComponent.h"
#include "Components/SphereComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/KismetSystemLibrary.h" // Overlap 체크용
#include "Kismet/GameplayStatics.h" // GetAllActorsWithTag (Fish 태그 액터 수집)
#include "XRMotionControllerBase.h"
#include "GameFramework/FloatingPawnMovement.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Components/SpotLightComponent.h"
#include "Components/AudioComponent.h"
#include "Components/TextRenderComponent.h"
#include "Sound/SoundBase.h"
#include "Engine/World.h"
#include "Engine/Engine.h" // GEngine->AddOnScreenDebugMessage (디버그용)
#include "DrawDebugHelpers.h" // 스캔 부채꼴 디버그 라인
#include "Engine/GameInstance.h"
#include "Engine/DataTable.h"
#include "Encyclopedia/EncyclopediaSubsystem.h"
#include "Encyclopedia/EncyclopediaLibrary.h"
#include "Encyclopedia/FishSwimComponent.h"
#include "Audio/BackgroundMusic.h"
#include "TimerManager.h"

AXRPawn::AXRPawn()
{
	PrimaryActorTick.bCanEverTick = true;

	MovementComp = CreateDefaultSubobject<UFloatingPawnMovement>(TEXT("MovementComp"));
	MovementComp->MaxSpeed = MoveSpeed; // 실제 이동 속도는 MaxSpeed가 결정 (아래 Input_Move 참고)

	// 0. 충돌 루트(캡슐) — FloatingPawnMovement는 "루트"만 스윕하므로,
	//    벽/바닥/BlockingVolume에 막히려면 루트 자체에 콜리전이 있어야 한다.
	CapsuleRoot = CreateDefaultSubobject<UCapsuleComponent>(TEXT("CapsuleRoot"));
	SetRootComponent(CapsuleRoot);
	CapsuleRoot->InitCapsuleSize(40.0f, 96.0f);         // 반지름 40, 반높이 96 (총 높이 약 192cm)
	CapsuleRoot->SetCollisionProfileName(TEXT("Pawn")); // 월드(WorldStatic/Dynamic, BlockingVolume)를 Block

	// 1. 기본 계층 구조 설정 (CapsuleRoot -> VRRoot -> Camera)
	VRRoot = CreateDefaultSubobject<USceneComponent>(TEXT("VRRoot"));
	VRRoot->SetupAttachment(CapsuleRoot);

	Camera = CreateDefaultSubobject<UCameraComponent>(TEXT("Camera"));
	Camera->SetupAttachment(VRRoot);
	// HMD(헤드셋) 위치/회전이 카메라를 직접 구동하도록 컨트롤러 회전 사용 안 함
	Camera->bUsePawnControlRotation = false;

	// 2. 모션 컨트롤러 설정 (왼손)
	LeftHandController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("LeftHandController"));
	LeftHandController->SetupAttachment(VRRoot);
	LeftHandController->SetTrackingMotionSource(FXRMotionControllerBase::LeftHandSourceId);

	LeftGrabSphere = CreateDefaultSubobject<USphereComponent>(TEXT("LeftGrabSphere"));
	LeftGrabSphere->SetupAttachment(LeftHandController);
	LeftGrabSphere->SetSphereRadius(10.0f); // 그랩 반경 설정
	LeftGrabSphere->SetCollisionProfileName(TEXT("Trigger")); // 물리 충돌 대신 트리거로 사용

	// 3. 모션 컨트롤러 설정 (오른손)
	RightHandController = CreateDefaultSubobject<UMotionControllerComponent>(TEXT("RightHandController"));
	RightHandController->SetupAttachment(VRRoot);
	RightHandController->SetTrackingMotionSource(FXRMotionControllerBase::RightHandSourceId);

	RightGrabSphere = CreateDefaultSubobject<USphereComponent>(TEXT("RightGrabSphere"));
	RightGrabSphere->SetupAttachment(RightHandController);
	RightGrabSphere->SetSphereRadius(10.0f);
	RightGrabSphere->SetCollisionProfileName(TEXT("Trigger"));

	// 4. 손전등 (오른손 컨트롤러에 부착) — 빛 연출 + 스캔 트레이스 방향 기준
	Flashlight = CreateDefaultSubobject<USpotLightComponent>(TEXT("Flashlight"));
	Flashlight->SetupAttachment(RightHandController);
	Flashlight->SetIntensity(5000.0f);
	Flashlight->SetAttenuationRadius(2000.0f);
	Flashlight->SetInnerConeAngle(12.0f);
	Flashlight->SetOuterConeAngle(25.0f);

	// 5. 스캔 루프 사운드용 오디오 컴포넌트 (오른손=스캐너 손에 부착). 시작 시 자동재생 끔.
	ScanLoopAudio = CreateDefaultSubobject<UAudioComponent>(TEXT("ScanLoopAudio"));
	ScanLoopAudio->SetupAttachment(RightHandController);
	ScanLoopAudio->bAutoActivate = false; // 스캔 시작할 때만 Play
	// 사운드 에셋이 루프가 아니어도 스캔 동안 끊기지 않게, 한 번 끝나면 다시 재생.
	ScanLoopAudio->OnAudioFinished.AddDynamic(this, &AXRPawn::OnScanLoopFinished);

	// 6. 수심 표시용 3D 텍스트 (왼손에 부착) — 손 화면 메쉬 위로 BP 에서 위치 맞추면 됨.
	DepthText = CreateDefaultSubobject<UTextRenderComponent>(TEXT("DepthText"));
	DepthText->SetupAttachment(LeftHandController);
	DepthText->SetHorizontalAlignment(EHTA_Center);
	DepthText->SetVerticalAlignment(EVRTA_TextCenter);
	DepthText->SetWorldSize(8.0f);                 // 글자 크기(작게 시작 — BP 에서 조정)
	DepthText->SetTextRenderColor(FColor::White);
	DepthText->SetText(FText::FromString(TEXT("Depth 0m")));
}

void AXRPawn::BeginPlay()
{
	Super::BeginPlay();

	// 디테일 패널에서 수정한 MoveSpeed를 런타임에 반영.
	// (생성자에서만 MaxSpeed를 설정하면 인스턴스에서 바꾼 MoveSpeed가 적용되지 않음)
	if (MovementComp)
	{
		MovementComp->MaxSpeed = MoveSpeed;
	}

	// 손전등/스캔 조준 보정 적용 — BP에서 Flashlight 컴포넌트를 돌려 맞춘 회전에
	// FlashlightAimOffset 을 "더한다"(덮어쓰지 않음). 기본값 0이면 BP 설정 그대로 사용.
	// 빛과 스캔 콘은 항상 같이 돌아간다(트레이스가 Flashlight forward 기준).
	if (Flashlight)
	{
		Flashlight->SetRelativeRotation(Flashlight->GetRelativeRotation() + FlashlightAimOffset);
	}

	// 도감 데이터테이블 자동 연결 — 디테일에서 EncyclopediaTable 을 지정하지 않았으면
	// 약속된 경로에서 DT_Encyclopedia 를 자동으로 찾아 쓴다(물고기별 ScanTime 등을 읽기 위함).
	// 디테일에서 다른 테이블을 지정했다면 그것을 그대로 존중한다.
	if (!EncyclopediaTable)
	{
		static const TCHAR* TablePaths[] =
		{
			TEXT("/Game/CH/Encyclopedia/DT_Encyclopedia.DT_Encyclopedia"),
			TEXT("/Game/Encyclopedia/DT_Encyclopedia.DT_Encyclopedia"),
		};
		for (const TCHAR* Path : TablePaths)
		{
			if (UDataTable* Found = LoadObject<UDataTable>(nullptr, Path))
			{
				EncyclopediaTable = Found;
				break;
			}
		}
	}

	// 소나(BP_Scanner)는 시작 시 자동 소환하지 않는다.
	// → Y 버튼(SpawnSonarAction)을 누르면 Input_SpawnSonar() 에서 소환한다.
}

void AXRPawn::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	// 손전등 스캔(물고기 수집) 갱신
	UpdateFlashlightScan(DeltaTime);

	// 손 화면 수심 텍스트 갱신 (값이 바뀐 프레임만 SetText)
	if (DepthText)
	{
		const int32 Meters = FMath::RoundToInt(GetPlayerDepthMeters());
		if (Meters != LastShownDepthMeters)
		{
			LastShownDepthMeters = Meters;
			DepthText->SetText(GetPlayerDepthText());
		}
	}
}

// ── 손전등 스캔(물고기 수집) ─────────────────────────────────────────────────
// 손전등 정면 원뿔(cone) 안에 들어온 "Fish" 태그 액터 중 가장 가까운 것을 반환한다.
//
// [변경 이유] 예전엔 얇은 라인 트레이스 9가닥을 쏴서 물고기에 "맞혀야" 했는데,
//   움직이는 물고기를 얇은 선으로 맞히기 어렵고, 콜리전 채널(ECC_Visibility)을
//   물고기 메시가 안 막거나 앞에 다른 물체가 있으면 감지가 실패했다.
//   → 이제는 콜리전에 의존하지 않고, 월드의 모든 "Fish" 태그 액터를 가져와
//      손전등 정면과의 "각도 + 거리"로 직접 판정한다. 훨씬 안정적으로 잡힌다.
AActor* AXRPawn::GetFlashlightScanTarget()
{
	if (!Flashlight || !GetWorld())
	{
		return nullptr;
	}

	// 스포트라이트는 +X(ForwardVector) 방향으로 빛을 쏜다.
	const FVector Start = Flashlight->GetComponentLocation();
	const FVector Forward = Flashlight->GetForwardVector();

	// 월드의 모든 "Fish" 태그 액터 수집
	TArray<AActor*> Fishes;
	UGameplayStatics::GetAllActorsWithTag(GetWorld(), FName("Fish"), Fishes);

	AActor* ClosestFish = nullptr;
	float ClosestDist = TNumericLimits<float>::Max();

	for (AActor* Fish : Fishes)
	{
		if (!Fish || Fish == this)
		{
			continue;
		}

		// 물고기 바운딩 박스 중심/크기 → 큰 물고기/가장자리도 잘 잡히도록 각도 여유를 준다.
		// bOnlyCollidingComponents=false → 콜리전이 꺼져 있어도 시각 메시 기준으로 바운드 계산
		FVector Origin, Extent;
		Fish->GetActorBounds(false, Origin, Extent);
		const float Radius = Extent.Size();

		const FVector ToFish = Origin - Start;
		const float Dist = ToFish.Size();
		if (Dist <= KINDA_SMALL_NUMBER || Dist > ScanRange)
		{
			continue; // 너무 가깝거나(겹침) 사거리 밖
		}

		const FVector ToFishDir = ToFish / Dist;
		const float CosAngle = FVector::DotProduct(Forward, ToFishDir);
		const float AngleDeg = FMath::RadiansToDegrees(FMath::Acos(FMath::Clamp(CosAngle, -1.0f, 1.0f)));

		// 물고기의 각반경(거리가 멀수록 작아짐)만큼 원뿔 각도에 여유를 더한다.
		const float AngularRadiusDeg = FMath::RadiansToDegrees(FMath::Asin(FMath::Clamp(Radius / Dist, 0.0f, 1.0f)));
		if (AngleDeg > ScanConeHalfAngle + AngularRadiusDeg)
		{
			continue; // 손전등 원뿔 밖
		}

		if (Dist < ClosestDist)
		{
			ClosestDist = Dist;
			ClosestFish = Fish;
		}

		// 디버그: 원뿔 안에 든 물고기를 향한 라인 (초록)
		if (bShowScanDebug)
		{
			DrawDebugLine(GetWorld(), Start, Origin, FColor::Green, false, -1.0f, 0, 0.5f);
		}
	}

	// 디버그: 손전등 정면 원뿔 + 중심선 시각화 + 대상이 없을 때 원인 파악용 메시지
	if (bShowScanDebug)
	{
		// 정면(조준) 중심선 — 굵은 시안색. 이 선이 장치가 가리키는 곳과 맞도록 FlashlightAimOffset 조정.
		DrawDebugLine(GetWorld(), Start, Start + Forward * ScanRange, FColor::Cyan, false, -1.0f, 0, 2.0f);

		DrawDebugCone(GetWorld(), Start, Forward, ScanRange,
			FMath::DegreesToRadians(ScanConeHalfAngle), FMath::DegreesToRadians(ScanConeHalfAngle),
			12, FColor(80, 80, 0), false, -1.0f, 0, 0.5f);

		if (bScanActive && !ClosestFish && GEngine)
		{
			// Fish 태그 액터가 0개면 → 물고기 BP에 "Fish" 태그가 안 붙은 것.
			GEngine->AddOnScreenDebugMessage(2, 0.1f, FColor::Yellow,
				FString::Printf(TEXT("스캔 대상 없음 (Fish 태그 액터 %d개)"), Fishes.Num()));
		}
	}

	return ClosestFish;
}

// 물고기의 헤엄 컴포넌트(있을 때만)에 도망 모드를 켜고/끈다.
//  - bFlee=true  : 이 폰(플레이어) 위치에서 멀어지는 쪽으로 빠르게 도망(FleeSpeed)
//  - bFlee=false : 평소 배회로 복귀
void AXRPawn::SetFishFleeing(AActor* Fish, bool bFlee)
{
	if (!Fish)
	{
		return;
	}
	if (UFishSwimComponent* Swim = Fish->FindComponentByClass<UFishSwimComponent>())
	{
		if (bFlee)
		{
			Swim->StartFleeFrom(GetActorLocation()); // 매 틱 호출 → 위협 위치 갱신
		}
		else
		{
			Swim->StopFleeing();
		}
	}
}

// 레벨의 배경음악(ABackgroundMusic)을 찾아 일시정지/재개한다.
void AXRPawn::SetBackgroundMusicPaused(bool bPaused)
{
	// 캐시가 없으면(또는 사라졌으면) 레벨에서 한 번 찾아 캐싱.
	if (!CachedBGM)
	{
		if (UWorld* World = GetWorld())
		{
			CachedBGM = Cast<ABackgroundMusic>(
				UGameplayStatics::GetActorOfClass(World, ABackgroundMusic::StaticClass()));
		}
	}
	if (CachedBGM)
	{
		CachedBGM->SetPaused(bPaused);
	}
}

// 스캔 중 루프 사운드 재생/정지 + 그동안 배경음악 일시정지.
void AXRPawn::SetScanLoopSound(bool bOn)
{
	// 스캔 사운드 유무와 상관없이, 스캔 중엔 배경음악을 멈추고 끝나면 이어서 재생.
	SetBackgroundMusicPaused(bOn);

	if (!ScanLoopAudio)
	{
		return;
	}
	if (bOn)
	{
		// 이미 재생 중이면 그대로 두고(다른 물고기로 옮겨도 끊김 없이 유지), 아니면 시작.
		if (ScanLoopSound && !ScanLoopAudio->IsPlaying())
		{
			ScanLoopAudio->SetSound(ScanLoopSound); // 런타임에 에셋이 바뀌어도 반영
			ScanLoopAudio->Play();
		}
	}
	else
	{
		if (ScanLoopAudio->IsPlaying())
		{
			ScanLoopAudio->Stop();
		}
	}
}

// 루프 사운드가 끝났을 때: 아직 스캔 중이면 다시 재생해서 끊김 없이 계속 들리게 한다.
// (사운드 에셋이 루프로 설정돼 있으면 이 이벤트가 안 와서 그냥 계속 재생됨 → 그래도 안전)
void AXRPawn::OnScanLoopFinished()
{
	if (bScanActive && CurrentScanTarget && ScanLoopAudio && ScanLoopSound)
	{
		ScanLoopAudio->Play();
	}
}

// 매 틱: 손전등이 켜져 있고 물고기를 비추고 있으면 진행도를 누적, 다 차면 수집 완료.
void AXRPawn::UpdateFlashlightScan(float DeltaTime)
{
	// 스캔 모드가 꺼져 있으면(오른손 트리거를 떼면) 스캔 중단/초기화
	if (!bScanActive)
	{
		if (CurrentScanTarget)
		{
			SetFishFleeing(CurrentScanTarget, false); // 도망 멈춤
			SetScanLoopSound(false);                  // 루프 사운드 정지
			CurrentScanTarget = nullptr;
			ScanProgress = 0.0f;
			ScanPercent = 0.0f;
			OnScanReset(); // 진행중이던 스캔이 끊김 → UI 숨김 신호
		}
		return;
	}

	// ── 락온(Lock-on) 방식 ───────────────────────────────────────────────
	// 아직 잡은 대상이 없으면: 손전등 콘 안의 물고기를 찾아 "락온"한다(스캔 시작).
	// 한번 락온하면 물고기가 도망쳐 콘을 벗어나도, 트리거를 누르고 있는 한 그 물고기로 계속 진행.
	if (!CurrentScanTarget)
	{
		AActor* Acquired = GetFlashlightScanTarget();
		if (!Acquired)
		{
			return; // 아직 아무 물고기도 안 비춤 → 대기
		}

		CurrentScanTarget = Acquired;
		ScanProgress = 0.0f;
		ScanPercent = 0.0f;

		// 이 물고기의 스캔 시간을 데이터테이블에서 읽어 적용(물고기마다 다르게).
		// 테이블/행/ScanTime 이 없으면 기본값 ScanDuration 으로 폴백.
		CurrentScanDuration = ScanDuration;
		if (EncyclopediaTable)
		{
			FName Row = UEncyclopediaLibrary::GetActorFishRowName(Acquired);
			Row = UEncyclopediaLibrary::ResolveEncyclopediaRowName(EncyclopediaTable, Row, Acquired);
			float RowScanTime = 0.0f;
			if (UEncyclopediaLibrary::GetRowScanTime(EncyclopediaTable, Row, RowScanTime))
			{
				CurrentScanDuration = RowScanTime;
			}
		}

		// 스캔 새로 시작 → 루프 사운드 재생 시작 + BP 이벤트(추가 연출용).
		SetScanLoopSound(true);
		OnScanStarted(Acquired, CurrentScanDuration);
	}

	// 락온한 대상이 사라졌으면(파괴 등) 초기화.
	if (!IsValid(CurrentScanTarget))
	{
		CurrentScanTarget = nullptr;
		ScanProgress = 0.0f;
		ScanPercent = 0.0f;
		SetScanLoopSound(false);
		OnScanReset();
		return;
	}

	// 락온된 대상으로 계속 진행(콘/사거리를 벗어나도 트리거 유지하는 한 유지).
	AActor* Target = CurrentScanTarget;

	// 스캔당하는 동안 물고기는 플레이어 반대쪽으로 빠르게 도망(매 틱 위협 위치 갱신).
	SetFishFleeing(Target, true);

	// 진행도 누적
	ScanProgress += DeltaTime;
	const float Percent = (CurrentScanDuration > 0.0f)
		? FMath::Clamp(ScanProgress / CurrentScanDuration, 0.0f, 1.0f)
		: 1.0f;

	// 화면 UI 가 바로 바인딩할 수 있도록 진행도(0~1) 를 변수에 반영 — 물고기별 시간 기준이라
	// 시간이 다 차면 자동으로 1.0(100%) 이 된다.
	ScanPercent = Percent;

	// 진행 UI 이벤트(BP) + 화면 메시지 (Key=1 로 매 틱 갱신)
	OnScanProgress(Target, Percent);
	if (bShowScanDebug && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(1, 0.1f, FColor::Cyan,
			FString::Printf(TEXT("스캔중... %d%%"), FMath::RoundToInt(Percent * 100.0f)));
	}

	// 완료 → BP에서 수집 처리, 같은 물고기 중복 방지 위해 리셋
	if (ScanProgress >= CurrentScanDuration)
	{
		ScanPercent = 1.0f; // 화면 UI 가 정확히 100% 로 마무리되도록 보장
		OnFishScanned(Target);

		// 수집 상태를 세션 저장소(GameInstanceSubsystem)에 영구 등록 → 도감을 한 바퀴 돌아도 유지된다.
		// 테이블이 지정돼 있으면 정확한 행 이름으로, 아니면 물고기 클래스 이름으로 등록(표준키로 변환됨).
		if (UGameInstance* GI = GetGameInstance())
		{
			if (UEncyclopediaSubsystem* Enc = GI->GetSubsystem<UEncyclopediaSubsystem>())
			{
				// 스폰된 물고기에 각인된 FishRowName 을 먼저 읽어 어떤 종인지 정확히 파악(없으면 NAME_None).
				FName Row = UEncyclopediaLibrary::GetActorFishRowName(Target);
				if (EncyclopediaTable)
				{
					Row = UEncyclopediaLibrary::ResolveEncyclopediaRowName(EncyclopediaTable, Row, Target);
				}
				Enc->MarkCollected(Row.IsNone() ? FName(*Target->GetClass()->GetName()) : Row);
			}
		}

		if (bShowScanDebug && GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Green, TEXT("수집 완료!"));
		}
		SetFishFleeing(Target, false); // 수집 완료 → 도망 멈춤
		SetScanLoopSound(false);       // 루프 사운드 정지
		if (ScanCompleteSound)         // 완료 사운드 1회 재생
		{
			UGameplayStatics::PlaySound2D(this, ScanCompleteSound);
		}
		CurrentScanTarget = nullptr;
		ScanProgress = 0.0f;

		// 완료 후 스캔 모드 자동 OFF → 같은 물고기를 곧바로 0%부터 다시 잡거나
		// 완료가 반복되는 걸 막는다. 다시 스캔하려면 트리거를 뗐다 다시 당기면 된다.
		bScanActive = false;
	}
}

// 소나음을 2D로 1회 재생하면서, 그동안 배경음을 끄고 사운드 길이만큼 뒤에 다시 켠다.
void AXRPawn::DuckBackgroundMusicForSound(USoundBase* Sound)
{
	if (!Sound)
	{
		return; // 사운드 없으면 BGM 도 건드리지 않음
	}

	UGameplayStatics::PlaySound2D(this, Sound);

	// 배경음 끄기
	SetBackgroundMusicPaused(true);

	// 사운드 길이만큼 뒤에 배경음 재개. 길이를 못 구하거나 루프(=음수/0)면 안전하게 2초로.
	float Dur = Sound->GetDuration();
	if (Dur <= 0.0f || Dur > 30.0f)
	{
		Dur = 2.0f;
	}
	if (UWorld* World = GetWorld())
	{
		World->GetTimerManager().SetTimer(
			SonarBGMResumeTimer, this, &AXRPawn::ResumeBackgroundMusicAfterSonar, Dur, false);
	}
}

// 소나음 종료 시 배경음 재개. 단, 그 사이 스캔이 시작됐다면(스캔이 BGM 을 꺼야 함) 켜지 않는다.
void AXRPawn::ResumeBackgroundMusicAfterSonar()
{
	if (CurrentScanTarget)
	{
		return; // 스캔 중이면 BGM 은 계속 꺼둔 채로
	}
	SetBackgroundMusicPaused(false);
}

// 소나 가동음 1회 재생(에셋이 지정돼 있을 때만) + 그동안 배경음 끄기.
void AXRPawn::PlaySonarActivateSound()
{
	DuckBackgroundMusicForSound(SonarActivateSound);
}

// 소나 감지음 1회 재생(에셋이 지정돼 있을 때만) + 그동안 배경음 끄기.
void AXRPawn::PlaySonarDetectSound()
{
	DuckBackgroundMusicForSound(SonarDetectSound);
}

// ── 소나(BP_Scanner) — Y 버튼 (보내준 BP의 T키 로직과 동일) ────────────────────
//  - 아직 소나가 없으면: 폰 위치에 BP_Scanner 스폰 (첫 입력 = 소환만)  [BP: Spawned? = false]
//  - 이미 있으면: 소나를 폰 위치로 이동(텔레포트) 후 DoScan 호출        [BP: Spawned? = true]
void AXRPawn::Input_SpawnSonar()
{
	if (!GetWorld())
	{
		return;
	}

	// 소나가 없으면 폰 위치에 스폰, 이미 있으면 폰 위치로 이동(텔레포트)
	if (!SonarInstance)
	{
		if (SonarClass)
		{
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = this;
			SpawnParams.Instigator = this;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
			SonarInstance = GetWorld()->SpawnActor<AActor>(SonarClass, FTransform(GetActorLocation()), SpawnParams);
		}

		if (!SonarInstance)
		{
			if (GEngine)
			{
				GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("[Sonar] 소환 실패 - Sonar Class 슬롯 확인"));
			}
			return;
		}

		// 새로 소환된 직후 1회 → BP에서 위젯에 ScannerRef 연결 등 처리
		OnSonarSpawned(SonarInstance);
	}
	else
	{
		SonarInstance->SetActorLocation(GetActorLocation(), false, nullptr, ETeleportType::TeleportPhysics);
	}

	// 소환/이동 직후 항상 DoScan 호출 → 한 번 누름에 화살표까지 바로 뜬다
	if (UFunction* DoScanFunc = SonarInstance->FindFunction(FName("DoScan")))
	{
		SonarInstance->ProcessEvent(DoScanFunc, nullptr);
		PlaySonarActivateSound(); // 소나 가동음 1회
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Cyan, TEXT("소나 가동..."));
		}
	}
	else if (GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 3.0f, FColor::Red, TEXT("[Sonar] DoScan 함수 없음 (BP_Sonar에 DoScan 이벤트 확인)"));
	}
}

// ── 입력 바인딩 ───────────────────────────────────────────────────────────
void AXRPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// 1. 빙의가 완료된 확실한 타이밍에 매핑 컨텍스트(IMC_VR) 장착!
	if (APlayerController* PC = Cast<APlayerController>(GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
		{
			if (VRMappingContext)
			{
				Subsystem->AddMappingContext(VRMappingContext, 0);
			}
		}
	}
	// EnhancedInputComponent로 캐스팅 (실패해도 크래시하지 않도록 Cast + null 체크)
	if (UEnhancedInputComponent* EnhancedInputComp = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// 이동 (조이스틱)
		if (MoveAction)
		{
			EnhancedInputComp->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AXRPawn::Input_Move);
		}

		// (왼손 그립 소나 가동은 제거됨 — 소나는 Y버튼으로 소환만 한다. 왼손 그립은 현재 미사용)

		// 오른손 트리거 = 스캔 (꾹 누르는 동안만 ON). Started=누름, Completed/Canceled=뗌
		if (GrabRightAction)
		{
			EnhancedInputComp->BindAction(GrabRightAction, ETriggerEvent::Started, this, &AXRPawn::StartScan);
			EnhancedInputComp->BindAction(GrabRightAction, ETriggerEvent::Completed, this, &AXRPawn::StopScan);
			EnhancedInputComp->BindAction(GrabRightAction, ETriggerEvent::Canceled, this, &AXRPawn::StopScan);
		}

		// 대시 (버튼 누르는 동안 부스트) — Started=누름, Completed/Canceled=뗌
		if (DashAction)
		{
			EnhancedInputComp->BindAction(DashAction, ETriggerEvent::Started, this, &AXRPawn::StartDash);
			EnhancedInputComp->BindAction(DashAction, ETriggerEvent::Completed, this, &AXRPawn::StopDash);
			EnhancedInputComp->BindAction(DashAction, ETriggerEvent::Canceled, this, &AXRPawn::StopDash);
		}

		// 손전등 온/오프 (X 버튼) — 누를 때마다 토글
		if (FlashlightAction)
		{
			EnhancedInputComp->BindAction(FlashlightAction, ETriggerEvent::Started, this, &AXRPawn::ToggleFlashlight);
		}

		// 도감 열기/닫기 (오른손 B 버튼)
		if (EncyclopediaToggleAction)
		{
			EnhancedInputComp->BindAction(EncyclopediaToggleAction, ETriggerEvent::Started, this, &AXRPawn::Input_EncyclopediaToggle);
		}

		// 도감 페이지 넘김 (오른손 썸스틱 X) — 한 번 꺾을 때마다 1페이지
		if (EncyclopediaPageAction)
		{
			EnhancedInputComp->BindAction(EncyclopediaPageAction, ETriggerEvent::Triggered, this, &AXRPawn::Input_EncyclopediaPage);
			EnhancedInputComp->BindAction(EncyclopediaPageAction, ETriggerEvent::Completed, this, &AXRPawn::Input_EncyclopediaPageReleased);
			EnhancedInputComp->BindAction(EncyclopediaPageAction, ETriggerEvent::Canceled, this, &AXRPawn::Input_EncyclopediaPageReleased);
		}

		// BP_Scanner 소환 (Y 버튼) — 누를 때 1회 소환
		if (SpawnSonarAction)
		{
			EnhancedInputComp->BindAction(SpawnSonarAction, ETriggerEvent::Started, this, &AXRPawn::Input_SpawnSonar);
		}
	}
}

// ── 수심 ────────────────────────────────────────────────────────────────────
// 현재 플레이어 수심(미터). 수면(WaterSurfaceZ)보다 아래로 내려간 깊이를 m 로 환산. 수면 위면 0.
float AXRPawn::GetPlayerDepthMeters() const
{
	const float DepthUnits = WaterSurfaceZ - GetActorLocation().Z; // 아래로 갈수록 +
	if (DepthUnits <= 0.0f || DepthUnitsPerMeter <= 0.0f)
	{
		return 0.0f; // 수면 위(또는 설정 오류)면 0
	}
	return DepthUnits / DepthUnitsPerMeter;
}

// "수심 12m" 형식 텍스트.
FText AXRPawn::GetPlayerDepthText() const
{
	const int32 Meters = FMath::RoundToInt(GetPlayerDepthMeters());
	return FText::FromString(FString::Printf(TEXT("Depth %dm"), Meters));
}

// ── 대시 ────────────────────────────────────────────────────────────────────
void AXRPawn::StartDash()
{
	bIsDashing = true;
	if (MovementComp)
	{
		MovementComp->MaxSpeed = DashSpeed;
	}
}

void AXRPawn::StopDash()
{
	bIsDashing = false;
	if (MovementComp)
	{
		MovementComp->MaxSpeed = MoveSpeed;
	}
}

// ── 입력 처리 함수 구현 ────────────────────────────────────────────────────

void AXRPawn::Input_Move(const FInputActionValue& Value)
{
	// 조이스틱의 Vector2D 값 가져오기
	FVector2D MoveVector = Value.Get<FVector2D>();

	if (Controller && (MoveVector.X != 0.0f || MoveVector.Y != 0.0f))
	{
		// 머리(카메라)가 실제로 바라보는 3D 방향으로 이동한다. (요구사항 #1)
		// Pitch 를 포함하므로, 고개를 아래로 숙이고 조이스틱을 앞으로 밀면 아래로 잠수,
		// 위를 보고 밀면 상승한다. (수영처럼 머리 각도로 상하 이동)
		// 스케일은 조이스틱 입력값(-1~1)만 전달 — 실제 속도/프레임보정은
		// FloatingPawnMovement(MaxSpeed)가 처리하므로 여기서 MoveSpeed/DeltaSeconds를 곱하지 않는다.
		const FVector ForwardDirection = Camera->GetForwardVector();
		AddMovementInput(ForwardDirection, MoveVector.Y);

		// 좌우 스트레이프 — 카메라 기준 오른쪽 방향
		const FVector RightDirection = Camera->GetRightVector();
		AddMovementInput(RightDirection, MoveVector.X);
	}
}

// 손전등 온/오프 토글 (X 버튼).
// 폰에 붙은 모든 스포트라이트를 함께 토글하므로, C++ Flashlight 든
// 블루프린트에서 추가한 HeadFlashlight/LightHand 든 같이 켜지고 꺼진다.
void AXRPawn::ToggleFlashlight()
{
	bFlashlightOn = !bFlashlightOn;

	TArray<USpotLightComponent*> SpotLights;
	GetComponents<USpotLightComponent>(SpotLights);
	for (USpotLightComponent* Spot : SpotLights)
	{
		if (Spot)
		{
			Spot->SetVisibility(bFlashlightOn);
		}
	}
}

// 오른손 트리거를 누르는 순간 → 스캔 시작.
// 누르고 있는 동안 UpdateFlashlightScan 이 물고기를 찾아 진행도를 누적한다.
void AXRPawn::StartScan()
{
	bScanActive = true;

	if (bShowScanDebug && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Green, TEXT("스캔 모드 ON"));
	}
}

// 오른손 트리거를 떼는 순간 → 스캔 종료 + 진행 중이던 스캔 초기화.
void AXRPawn::StopScan()
{
	bScanActive = false;

	// 스캔 진행 중에 트리거를 떼면(취소) → 도망/사운드 정지 + 진행중 UI 숨김 신호(OnScanReset).
	// (이걸 안 부르면 진행도 UI 가 화면에 남아 안 사라진다.)
	if (CurrentScanTarget)
	{
		SetFishFleeing(CurrentScanTarget, false); // 쫓던 물고기 도망 멈춤
		SetScanLoopSound(false);                  // 루프 사운드 정지 + 배경음 재개
		CurrentScanTarget = nullptr;
		ScanProgress = 0.0f;
		ScanPercent = 0.0f;
		OnScanReset();                            // 진행중 UI 숨김
	}

	if (bShowScanDebug && GEngine)
	{
		GEngine->AddOnScreenDebugMessage(-1, 2.0f, FColor::Orange, TEXT("스캔 모드 OFF"));
	}
}

// ── 홀로그램 도감 입력 ───────────────────────────────────────────────────────
// 오른손 B = 열기/닫기. 실제 동작은 블루프린트의 OnToggleEncyclopedia 에서 구현.
void AXRPawn::Input_EncyclopediaToggle()
{
	OnToggleEncyclopedia();
}

// 오른손 썸스틱 X = 페이지 넘김.
// 스틱을 한 번 꺾으면 1페이지만 넘기고, 중앙으로 돌아와야 다시 넘길 수 있다(연속 넘김 방지).
void AXRPawn::Input_EncyclopediaPage(const FInputActionValue& Value)
{
	const float Axis = Value.Get<float>();
	const float FireThreshold = 0.7f;  // 이 이상 꺾여야 페이지 넘김 발동
	const float ResetThreshold = 0.3f; // 이 아래로 돌아오면 다시 넘길 수 있게 해제

	if (FMath::Abs(Axis) < ResetThreshold)
	{
		bPageFlickEngaged = false;
	}
	else if (!bPageFlickEngaged && FMath::Abs(Axis) >= FireThreshold)
	{
		bPageFlickEngaged = true;
		OnEncyclopediaPage(Axis > 0.0f ? 1 : -1); // 오른쪽=다음(+1), 왼쪽=이전(-1)
	}
}

void AXRPawn::Input_EncyclopediaPageReleased()
{
	bPageFlickEngaged = false;
}

// 조이스틱 "한 번 꺾을 때 1회" 디바운스. BP의 IA_EncyclopediaRow 이벤트(Triggered)에서
// 매 프레임 축값을 넣어 호출 → 반환값이 0이 아닐 때만 페이지를 넘기면 연속 넘김이 사라진다.
int32 AXRPawn::ConsumeAxisFlick(float AxisValue, float FireThreshold, float ResetThreshold)
{
	// 응답성을 위해 임계값을 함수 안에서 고정한다. (이미 배치된 BP 노드에 박혀있는 옛 기본값에
	// 휘둘리지 않도록 — BP 수정 없이 여기서 결정.)
	//   발동 0.8 이상 / 재무장 0.5 미만 → 0.8까지 꺾으면 1회 발동, 0.5 아래로 돌아오면 다시 발동 가능.
	FireThreshold = 0.8f;
	ResetThreshold = 0.5f;

	// 이전 호출보다 2프레임 이상 지나 들어왔다 = 그 사이 스틱이 중앙으로 가서 Triggered 가
	// 끊겼던 것 → "놨다 다시 꺾음"으로 보고 플래그 해제. (Triggered 가 저값에서 안 와서
	// |Axis|<Reset 리셋만으로는 영영 안 풀리는 문제를 보완)
	const uint64 Frame = GFrameCounter;
	if (Frame > LastFlickFrame + 1)
	{
		bPageFlickEngaged = false;
	}
	LastFlickFrame = Frame;

	// 중앙 근처로 돌아오면 다시 발동 가능 상태로 해제
	if (FMath::Abs(AxisValue) < ResetThreshold)
	{
		bPageFlickEngaged = false;
		return 0;
	}

	// 아직 발동 안 했고, 충분히 꺾였으면 → 이번에 1회만 발동
	if (!bPageFlickEngaged && FMath::Abs(AxisValue) >= FireThreshold)
	{
		bPageFlickEngaged = true;
		return (AxisValue > 0.0f) ? 1 : -1; // 오른쪽=+1(다음), 왼쪽=-1(이전)
	}

	// 꺾은 채 유지 중 → 추가 발동 없음
	return 0;
}

// ── 홀로그램 손 따라다니기 ───────────────────────────────────────────────────
// 홀로그램 액터를 손 컨트롤러에 붙인다. 붙는 순간부터 손을 따라 같이 움직인다(매 틱 코드 불필요).
void AXRPawn::AttachHologramToHand(AActor* Hologram, bool bRightHand, FVector LocalOffset)
{
	if (!Hologram)
	{
		return;
	}

	UMotionControllerComponent* Hand = bRightHand ? RightHandController : LeftHandController;
	if (!Hand)
	{
		return;
	}

	// 손 위치/회전에 스냅해서 붙인 뒤, LocalOffset 만큼 손에서 떨어뜨린다.
	Hologram->AttachToComponent(Hand, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
	Hologram->SetActorRelativeLocation(LocalOffset);
}

// 손에서 떼어내 지금 있는 자리(월드 위치)에 그대로 둔다.
void AXRPawn::DetachHologram(AActor* Hologram)
{
	if (Hologram)
	{
		Hologram->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);
	}
}

// 오른쪽 그랩 처리
void AXRPawn::Input_GrabRight_Started() { AttemptGrab(RightGrabSphere, RightHandController); }
void AXRPawn::Input_GrabRight_Completed() { ReleaseGrab(RightHandController); }


// ── 그랩 로직 구현 (간단한 버전) ────────────────────────────────────────────

void AXRPawn::AttemptGrab(USphereComponent* GrabSphere, UMotionControllerComponent* TargetController)
{
	// 이미 잡고 있다면 리턴
	if ((TargetController == LeftHandController && HeldActorLeft) || (TargetController == RightHandController && HeldActorRight))
	{
		return;
	}

	// 1. GrabSphere 영역 내에 있는 액터 찾기 (Sphere Overlap Actor)
	TArray<AActor*> OverlappingActors;
	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add(UEngineTypes::ConvertToObjectType(ECollisionChannel::ECC_PhysicsBody)); // 물리 액터만 감지

	// Kismet 라이브러리를 사용하여 간단하게 Overlap 체크
	UKismetSystemLibrary::SphereOverlapActors(
		GrabSphere,
		GrabSphere->GetComponentLocation(),
		GrabSphere->GetScaledSphereRadius(),
		ObjectTypes,
		AActor::StaticClass(), // 모든 액터 클래스
		TArray<AActor*>(), // 무시할 액터 없음
		OverlappingActors
	);

	// 2. 가장 가까운 액터 찾기 및 Attach
	AActor* ClosestActor = nullptr;
	float MinDistance = FLT_MAX;

	for (AActor* Actor : OverlappingActors)
	{
		// 자기 자신(Pawn)은 무시
		if (Actor == this) continue;

		// 액터의 스태틱 메쉬가 있는지 확인하고 물리 시뮬레이션 중인지 체크 (잡으려면 물리가 켜져 있어야 함)
		UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(Actor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (MeshComp && MeshComp->IsSimulatingPhysics())
		{
			float Distance = FVector::Dist(GrabSphere->GetComponentLocation(), Actor->GetActorLocation());
			if (Distance < MinDistance)
			{
				MinDistance = Distance;
				ClosestActor = Actor;
			}
		}
	}

	if (ClosestActor)
	{
		// 3. 액터 Attach (잡기)
		UE_LOG(LogTemp, Warning, TEXT("Grabbed Actor: %s"), *ClosestActor->GetName());

		// 스태틱 메쉬의 물리를 끄고 컨트롤러에 Attach
		UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(ClosestActor->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (MeshComp)
		{
			MeshComp->SetSimulatePhysics(false); // 잡았을 때는 물리를 꺼야 함
		}

		// 컨트롤러 컴포넌트에 액터를 Attach (위치, 회전 유지)
		ClosestActor->AttachToComponent(TargetController, FAttachmentTransformRules::KeepWorldTransform);

		// 잡고 있는 액터 변수에 저장
		if (TargetController == LeftHandController) { HeldActorLeft = ClosestActor; }
		else { HeldActorRight = ClosestActor; }
	}
}

void AXRPawn::ReleaseGrab(UMotionControllerComponent* TargetController)
{
	AActor* ActorToRelease = (TargetController == LeftHandController) ? HeldActorLeft : HeldActorRight;

	if (ActorToRelease)
	{
		UE_LOG(LogTemp, Warning, TEXT("Released Actor: %s"), *ActorToRelease->GetName());

		// 1. Detach (떼어내기)
		ActorToRelease->DetachFromActor(FDetachmentTransformRules::KeepWorldTransform);

		// 2. 물리 다시 켜기
		UStaticMeshComponent* MeshComp = Cast<UStaticMeshComponent>(ActorToRelease->GetComponentByClass(UStaticMeshComponent::StaticClass()));
		if (MeshComp)
		{
			MeshComp->SetSimulatePhysics(true);
		}

		// 3. 변수 초기화
		if (TargetController == LeftHandController) { HeldActorLeft = nullptr; }
		else { HeldActorRight = nullptr; }
	}
}
