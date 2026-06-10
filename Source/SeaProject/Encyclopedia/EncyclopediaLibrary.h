// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "EncyclopediaLibrary.generated.h"

class UDataTable;
class UTextRenderComponent;
class UFont;
class USkeletalMesh;
class UAnimSequenceBase;
class AActor;

/**
 * 도감(Encyclopedia) 헬퍼.
 *
 * 문제: BP_HologramDisplay 가 GetDataTableRowFromName 으로 행을 찾을 때, 물고기가 넘기는
 *       이름과 DT_Encyclopedia 의 행 이름이 대소문자/접두어("BP_")/오타로 어긋나면 조회가
 *       실패(RowNotFound)하고 TextRender 가 기본값 "Text" 를 그대로 보여준다.
 *
 * 해결: 넘어온 이름을 "정규화"해서 표를 유연하게 뒤져 올바른 "실제 행 이름"을 돌려준다.
 *       BP 에서는 이 함수의 결과(FName)를 GetDataTableRowFromName 의 Row Name 으로 넣으면 끝.
 */
UCLASS()
class SEAPROJECT_API UEncyclopediaLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * 요청한 이름(RequestedRow)·물고기 액터 이름을 토대로 DataTable 에서 가장 잘 맞는 "실제 행 이름"을 찾는다.
	 * 매칭 우선순위: 1) 완전일치 → 2) 대소문자/기호 무시 일치 → 3) 한쪽이 다른 쪽으로 시작 → 4) 1글자 오타(유일할 때).
	 *
	 * @param DataTable     도감 데이터테이블(DT_Encyclopedia).
	 * @param RequestedRow  물고기가 넘긴 행 이름(대소문자/오타 가능). 비어있으면 FishActor 이름으로 추정.
	 * @param FishActor     (선택) 행 이름이 없을 때 클래스 이름("BP_Shark"→"shark")에서 추정하는 데 사용.
	 * @return              표에 존재하는 실제 행 이름. 못 찾으면 NAME_None.
	 */
	UFUNCTION(BlueprintCallable, Category = "Encyclopedia")
	static FName ResolveEncyclopediaRowName(const UDataTable* DataTable, FName RequestedRow, AActor* FishActor = nullptr);

	/** 정규화: 소문자로 바꾸고 영문/숫자만 남긴다. ("BP_Shark_2" → "bpshark2") */
	UFUNCTION(BlueprintPure, Category = "Encyclopedia")
	static FString NormalizeName(const FString& In);

	/**
	 * 수집 상태 저장/조회용 "표준 키". 접두어("BP_")·번호·대소문자·기호 차이를 모두 흡수한다.
	 * 예: "BP_Shark_C_2", "Shark", "shark" → 전부 같은 키 "shark".
	 * (DT 행 이름이든 물고기 클래스 이름이든 같은 키로 모이게 해서 수집 매칭이 어긋나지 않게 함)
	 */
	UFUNCTION(BlueprintPure, Category = "Encyclopedia")
	static FName ToCanonicalKey(FName RowName);

	/**
	 * 도감 한 행을 읽어 "이름 / 스테이지 / 깊이" 형식 텍스트를 만들고, (한글 폰트까지 적용해)
	 * TextRender 컴포넌트에 한 번에 표시한다. BP에서는 이 노드 하나만 호출하면 끝.
	 *
	 * @param Target       표시할 대상 TextRender 컴포넌트(BP_HologramDisplay 의 그 컴포넌트).
	 * @param DataTable    DT_Encyclopedia.
	 * @param RowName      물고기가 넘긴 행 이름(대소문자/오타 자동 보정됨).
	 * @param KoreanFont   (선택) 한글 지원 Font 에셋. 넣으면 TextRender 폰트로 적용 → 한글이 보인다.
	 * @param FishActor    (선택) 행 이름이 비었을 때 물고기 클래스 이름으로 추정.
	 * @return             행을 찾아 표시에 성공하면 true.
	 */
	UFUNCTION(BlueprintCallable, Category = "Encyclopedia")
	static bool ShowEncyclopediaText(UTextRenderComponent* Target, const UDataTable* DataTable, FName RowName, UFont* KoreanFont = nullptr, AActor* FishActor = nullptr);

	/**
	 * 위 ShowEncyclopediaText 와 같지만, "이름 / 스테이지 / 깊이" 를 한 줄로 합치지 않고
	 * 세 개의 TextRender 컴포넌트에 따로 표시한다(홀로그램처럼 Name/Stage/Depth 를 분리 배치할 때).
	 * 각 대상은 nullptr 이어도 되며(그 항목은 건너뜀), 한글 폰트는 세 컴포넌트 모두에 적용한다.
	 *   - NameText  → 종 이름(없으면 행 이름)
	 *   - StageText → "스테이지 N" (StageLevel 필드 없으면 빈 텍스트)
	 *   - DepthText → "깊이 Nm"   (minDepth/maxDepth 있으면 중간값, 없으면 단일 Depth)
	 *
	 * @return  행을 찾아 표시에 성공하면 true.
	 */
	UFUNCTION(BlueprintCallable, Category = "Encyclopedia")
	static bool ShowEncyclopediaFields(UTextRenderComponent* NameText, UTextRenderComponent* StageText, UTextRenderComponent* DepthText, const UDataTable* DataTable, FName RowName, UFont* KoreanFont = nullptr, AActor* FishActor = nullptr);

	/**
	 * TextRender 의 글자 크기(World Size)를 자동 조절해, 가로 폭이 MaxWidth(언리얼 로컬 단위, cm) 를
	 * 넘지 않게 맞춘다. 짧으면 MaxSize 그대로, 길면 MinSize 까지 비례해서 줄인다.
	 * (긴 물고기 이름이 시계 화면 밖으로 나가는 것을 방지) — 호출 전에 텍스트/폰트가 세팅돼 있어야 정확하다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Encyclopedia")
	static void FitTextRenderToWidth(UTextRenderComponent* Text, float MaxWidth, float MaxSize, float MinSize = 1.0f);

	/**
	 * 도감 한 행을 "이름 / 스테이지 / 깊이" 형식 FText 로 조립해서 돌려준다.
	 * (TextRender 말고 UMG Text Block 등에 직접 쓰고 싶을 때 사용. 폰트는 위젯에서 지정)
	 */
	UFUNCTION(BlueprintCallable, Category = "Encyclopedia")
	static FText BuildEncyclopediaText(const UDataTable* DataTable, FName RowName, AActor* FishActor = nullptr);

	// ── 자동 스폰용 행 데이터 읽기 ────────────────────────────────────────────────
	// UserDefinedStruct 라 필드 이름이 꾸며져 있어, 친근한 이름("Depth","ModelMesh"...)으로 리플렉션 조회한다.

	/** 행에서 Depth(수심, 미터) 를 읽는다. 못 읽으면 false. (RowName 은 대소문자/오타 보정됨) */
	UFUNCTION(BlueprintCallable, Category = "Encyclopedia")
	static bool GetRowDepthMeters(const UDataTable* DataTable, FName RowName, float& OutDepthMeters);

	/** 행에서 수심 범위(minDepth ~ maxDepth, 미터) 를 읽는다. min/max 필드가 없으면 단일 Depth 로 폴백
	 *  (그 경우 Min==Max). 둘 다 없으면 false. 스폰을 이 범위 안 랜덤 수심으로 흩뿌리는 데 사용. */
	UFUNCTION(BlueprintCallable, Category = "Encyclopedia")
	static bool GetRowDepthRange(const UDataTable* DataTable, FName RowName, float& OutMinMeters, float& OutMaxMeters);

	/** 행에서 SpawnCount(스폰 마리 수) 를 읽는다. 구조체에 그 필드가 있고 값이 있으면 true.
	 *  (구조체에 SpawnCount 필드를 추가하면 마리 수를 데이터로 관리 가능) */
	UFUNCTION(BlueprintCallable, Category = "Encyclopedia")
	static bool GetRowSpawnCount(const UDataTable* DataTable, FName RowName, int32& OutCount);

	/** 행에서 ScanTime(스캔 완료까지 걸리는 시간, 초) 를 읽는다. 구조체에 그 필드가 있고 값이 0보다 크면 true.
	 *  (물고기마다 스캔에 걸리는 시간을 데이터로 다르게 관리하는 데 사용 — RowName 은 대소문자/오타 보정됨) */
	UFUNCTION(BlueprintCallable, Category = "Encyclopedia")
	static bool GetRowScanTime(const UDataTable* DataTable, FName RowName, float& OutScanTime);

	/** 행의 ModelMesh(SkeletalMesh) 를 읽는다. 소프트 참조면 동기 로드한다. 없으면 nullptr. */
	UFUNCTION(BlueprintCallable, Category = "Encyclopedia")
	static USkeletalMesh* GetRowModelMesh(const UDataTable* DataTable, FName RowName);

	/** 행의 AnimationSequence 를 읽는다. 소프트 참조면 동기 로드한다. 없으면 nullptr. */
	UFUNCTION(BlueprintCallable, Category = "Encyclopedia")
	static UAnimSequenceBase* GetRowAnimSequence(const UDataTable* DataTable, FName RowName);

	/** 물고기 액터의 "FishRowName" 프로퍼티(Name/String/Text 무엇이든)를 읽어 FName 으로 돌려준다. 없으면 NAME_None.
	 *  (스캔 완료 시 어떤 종인지 정확히 알아내는 데 사용 — BP_FishBase 든 C++ 든 그 변수만 있으면 됨) */
	UFUNCTION(BlueprintCallable, Category = "Encyclopedia")
	static FName GetActorFishRowName(AActor* Actor);

	/** 물고기 액터의 "FishRowName" 프로퍼티에 값을 써넣는다(있을 때만). 스포너가 종류를 각인하는 데 사용. */
	UFUNCTION(BlueprintCallable, Category = "Encyclopedia")
	static bool SetActorFishRowName(AActor* Actor, FName RowName);

	/** 물고기 정면(+X)에 벽(WorldStatic)이 CheckDistance(cm) 안에 있으면 true 와 "피할 방향(Yaw, 도)"을 준다.
	 *  BP Event Tick 에서 호출 → true 면 TargetRotation 의 Yaw 를 OutAvoidYaw 로 세팅하면 벽을 피해 돈다.
	 *  (부딪히기 전에 미리 트는 방식이라 벽에 박혀 멈추지 않음) */
	UFUNCTION(BlueprintCallable, Category = "Encyclopedia")
	static bool CheckWallAhead(AActor* Fish, float CheckDistance, float CheckRadius, float& OutAvoidYaw);
};
