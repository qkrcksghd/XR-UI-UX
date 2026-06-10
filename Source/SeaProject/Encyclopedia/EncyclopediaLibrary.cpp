// Fill out your copyright notice in the Description page of Project Settings.

#include "EncyclopediaLibrary.h"
#include "Engine/DataTable.h"
#include "GameFramework/Actor.h"
#include "Components/TextRenderComponent.h"
#include "Engine/Font.h"
#include "Engine/SkeletalMesh.h"
#include "Animation/AnimSequenceBase.h"
#include "UObject/UnrealType.h"
#include "UObject/TextProperty.h"
#include "UObject/SoftObjectPtr.h"
#include "Engine/World.h"
#include "CollisionQueryParams.h"

namespace
{
	// "BP_Shark_C_2" 같은 이름에서 접두어/접미어를 떼어 핵심만 남긴다.
	//  - 앞의 "BP_" 접두어 제거
	//  - 뒤의 "_C"(블루프린트 클래스 접미어) 제거
	//  - 뒤의 "_<숫자>"(인스턴스 번호) 제거
	FString StripDecorations(FString Name)
	{
		Name.TrimStartAndEndInline();

		// 앞 "BP_" (대소문자 무시)
		if (Name.StartsWith(TEXT("BP_"), ESearchCase::IgnoreCase))
		{
			Name.RightChopInline(3);
		}

		// 뒤 "_C"
		if (Name.EndsWith(TEXT("_C"), ESearchCase::IgnoreCase))
		{
			Name.LeftChopInline(2);
		}

		// 뒤 "_숫자" 반복 제거 (예: "_C_2" → "_2" 까지 처리되도록 루프)
		int32 Underscore;
		while (Name.FindLastChar(TEXT('_'), Underscore))
		{
			const FString Tail = Name.Mid(Underscore + 1);
			bool bAllDigits = Tail.Len() > 0;
			for (const TCHAR C : Tail)
			{
				if (!FChar::IsDigit(C)) { bAllDigits = false; break; }
			}
			if (bAllDigits) { Name.LeftInline(Underscore); }
			else { break; }
		}

		return Name;
	}

	// 유저 정의 구조체(UserDefinedStruct)의 행 데이터에서 "친근한 이름"(예 "Name", "StageLevel",
	// "Depth")으로 필드를 찾아 문자열로 읽는다. 못 찾으면 false.
	//  - UserDefinedStruct 의 실제 프로퍼티 이름은 "Name_2_GUID" 처럼 꾸며져 있으므로
	//    GetAuthoredNameForField 로 원래 이름을 얻어 비교한다.
	bool ReadFieldAsString(const UScriptStruct* Struct, const uint8* Data, const FString& Friendly, FString& Out)
	{
		if (!Struct || !Data)
		{
			return false;
		}

		for (TFieldIterator<FProperty> It(Struct); It; ++It)
		{
			FProperty* Prop = *It;
			const FString Authored = Struct->GetAuthoredNameForField(Prop);
			if (!Authored.Equals(Friendly, ESearchCase::IgnoreCase))
			{
				continue;
			}

			const void* ValuePtr = Prop->ContainerPtrToValuePtr<void>(Data);

			// 자주 쓰는 타입은 보기 좋게 직접 변환
			if (const FTextProperty* TextProp = CastField<FTextProperty>(Prop))
			{
				Out = TextProp->GetPropertyValue(ValuePtr).ToString();
			}
			else if (const FNameProperty* NameProp = CastField<FNameProperty>(Prop))
			{
				Out = NameProp->GetPropertyValue(ValuePtr).ToString();
			}
			else if (const FStrProperty* StrProp = CastField<FStrProperty>(Prop))
			{
				Out = StrProp->GetPropertyValue(ValuePtr);
			}
			else if (const FIntProperty* IntProp = CastField<FIntProperty>(Prop))
			{
				Out = FString::FromInt(IntProp->GetPropertyValue(ValuePtr));
			}
			else if (const FInt64Property* Int64Prop = CastField<FInt64Property>(Prop))
			{
				Out = FString::Printf(TEXT("%lld"), Int64Prop->GetPropertyValue(ValuePtr));
			}
			else if (const FByteProperty* ByteProp = CastField<FByteProperty>(Prop))
			{
				Out = FString::FromInt(ByteProp->GetPropertyValue(ValuePtr));
			}
			else if (const FFloatProperty* FloatProp = CastField<FFloatProperty>(Prop))
			{
				Out = FString::SanitizeFloat(FloatProp->GetPropertyValue(ValuePtr));
			}
			else if (const FDoubleProperty* DoubleProp = CastField<FDoubleProperty>(Prop))
			{
				Out = FString::SanitizeFloat(DoubleProp->GetPropertyValue(ValuePtr));
			}
			else if (const FBoolProperty* BoolProp = CastField<FBoolProperty>(Prop))
			{
				Out = BoolProp->GetPropertyValue(ValuePtr) ? TEXT("true") : TEXT("false");
			}
			else
			{
				// 그 외 타입은 엔진 기본 텍스트 변환에 맡긴다.
				Prop->ExportTextItem_Direct(Out, ValuePtr, nullptr, nullptr, PPF_None);
			}
			return true;
		}
		return false;
	}

	// "12.0", "12.34000" 같은 숫자 문자열을 보기 좋게 다듬는다(불필요한 0 제거).
	FString TidyNumber(const FString& In)
	{
		if (!In.Contains(TEXT("."))) { return In; }
		FString S = In;
		while (S.EndsWith(TEXT("0"))) { S.LeftChopInline(1); }
		if (S.EndsWith(TEXT("."))) { S.LeftChopInline(1); }
		return S;
	}

	// 한글 폰트를 결정한다. 인자로 받은 게 있으면 그대로, 없으면 "약속된 경로"에서 찾아본다(없으면 nullptr).
	// TextRender 기본 폰트(Roboto)엔 한글 글리프가 없어서, 한글을 보이려면 이 폰트가 필요하다.
	UFont* ResolveKoreanFont(UFont* InFont)
	{
		if (InFont)
		{
			return InFont;
		}
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
				return Found;
			}
		}
		return nullptr;
	}

	// 행의 수심 표시 문자열: minDepth/maxDepth 가 있으면 중간값(반올림), 없으면 단일 Depth. 둘 다 없으면 빈 문자열.
	FString BuildDepthDisplay(const UScriptStruct* RowStruct, const uint8* RowData)
	{
		FString MinStr, MaxStr;
		const bool bHasMin = ReadFieldAsString(RowStruct, RowData, TEXT("minDepth"), MinStr) && !MinStr.IsEmpty();
		const bool bHasMax = ReadFieldAsString(RowStruct, RowData, TEXT("maxDepth"), MaxStr) && !MaxStr.IsEmpty();
		if (bHasMin || bHasMax)
		{
			const float Mn = bHasMin ? FCString::Atof(*MinStr) : FCString::Atof(*MaxStr);
			const float Mx = bHasMax ? FCString::Atof(*MaxStr) : FCString::Atof(*MinStr);
			return FString::FromInt(FMath::RoundToInt((Mn + Mx) * 0.5f)); // 중간값(반올림)
		}
		FString DepthStr;
		ReadFieldAsString(RowStruct, RowData, TEXT("Depth"), DepthStr); // 폴백: 단일 Depth
		return TidyNumber(DepthStr);
	}

	// 두 문자열의 레벤슈타인(편집) 거리. 오타 1글자 판정용.
	int32 EditDistance(const FString& A, const FString& B)
	{
		const int32 LenA = A.Len();
		const int32 LenB = B.Len();
		if (LenA == 0) { return LenB; }
		if (LenB == 0) { return LenA; }

		TArray<int32> Prev, Curr;
		Prev.SetNumUninitialized(LenB + 1);
		Curr.SetNumUninitialized(LenB + 1);
		for (int32 j = 0; j <= LenB; ++j) { Prev[j] = j; }

		for (int32 i = 1; i <= LenA; ++i)
		{
			Curr[0] = i;
			for (int32 j = 1; j <= LenB; ++j)
			{
				const int32 Cost = (A[i - 1] == B[j - 1]) ? 0 : 1;
				Curr[j] = FMath::Min3(Prev[j] + 1, Curr[j - 1] + 1, Prev[j - 1] + Cost);
			}
			Prev = Curr;
		}
		return Prev[LenB];
	}
}

FString UEncyclopediaLibrary::NormalizeName(const FString& In)
{
	FString Out;
	Out.Reserve(In.Len());
	for (const TCHAR C : In)
	{
		if (FChar::IsAlnum(C))
		{
			Out.AppendChar(FChar::ToLower(C));
		}
	}
	return Out;
}

FName UEncyclopediaLibrary::ToCanonicalKey(FName RowName)
{
	if (RowName.IsNone())
	{
		return NAME_None;
	}
	const FString Key = NormalizeName(StripDecorations(RowName.ToString()));
	return Key.IsEmpty() ? NAME_None : FName(*Key);
}

FName UEncyclopediaLibrary::ResolveEncyclopediaRowName(const UDataTable* DataTable, FName RequestedRow, AActor* FishActor)
{
	if (!DataTable)
	{
		return NAME_None;
	}

	// 표의 실제 행 이름 목록
	const TArray<FName> RowNames = DataTable->GetRowNames();
	if (RowNames.Num() == 0)
	{
		return NAME_None;
	}

	// 후보 이름들을 우선순위대로 모은다.
	//  1순위: 물고기가 직접 넘긴 RequestedRow
	//  2순위: 물고기 액터 클래스 이름에서 추정 (예: BP_Shark → shark)
	TArray<FString> Candidates;
	if (!RequestedRow.IsNone())
	{
		Candidates.Add(StripDecorations(RequestedRow.ToString()));
	}
	if (FishActor)
	{
		if (const UClass* Cls = FishActor->GetClass())
		{
			Candidates.Add(StripDecorations(Cls->GetName()));
		}
		Candidates.Add(StripDecorations(FishActor->GetName()));
	}

	if (Candidates.Num() == 0)
	{
		return NAME_None;
	}

	// 행 이름들을 정규화해 미리 캐시 (정규화값 → 실제 FName)
	TArray<FString> NormRows;
	NormRows.Reserve(RowNames.Num());
	for (const FName& Row : RowNames)
	{
		NormRows.Add(NormalizeName(Row.ToString()));
	}

	// ── 1) 완전 일치 (대소문자까지 그대로) ─────────────────────────────
	for (const FString& Cand : Candidates)
	{
		const FName AsName(*Cand);
		if (RowNames.Contains(AsName))
		{
			return AsName;
		}
	}

	// ── 2) 정규화 일치 (대소문자/기호/접두어 무시) ─────────────────────
	for (const FString& Cand : Candidates)
	{
		const FString NCand = NormalizeName(Cand);
		if (NCand.IsEmpty()) { continue; }
		for (int32 i = 0; i < RowNames.Num(); ++i)
		{
			if (NormRows[i] == NCand)
			{
				return RowNames[i];
			}
		}
	}

	// ── 3) 한쪽이 다른 쪽으로 시작 (부분 일치) ─────────────────────────
	//   예: "sharkfish" ↔ "shark". 너무 짧은 건 오매칭 방지로 제외.
	for (const FString& Cand : Candidates)
	{
		const FString NCand = NormalizeName(Cand);
		if (NCand.Len() < 4) { continue; }
		for (int32 i = 0; i < RowNames.Num(); ++i)
		{
			const FString& NRow = NormRows[i];
			if (NRow.Len() < 4) { continue; }
			if (NRow.StartsWith(NCand) || NCand.StartsWith(NRow))
			{
				return RowNames[i];
			}
		}
	}

	// ── 4) 오타 1글자 (편집거리 ≤1) — 단, 그런 행이 "유일"할 때만 ────────
	//   예: "mandarine" ↔ "madarine". 후보가 둘 이상이면 위험하니 포기.
	for (const FString& Cand : Candidates)
	{
		const FString NCand = NormalizeName(Cand);
		if (NCand.Len() < 4) { continue; }

		int32 BestIdx = INDEX_NONE;
		int32 MatchCount = 0;
		for (int32 i = 0; i < RowNames.Num(); ++i)
		{
			if (EditDistance(NCand, NormRows[i]) <= 1)
			{
				++MatchCount;
				BestIdx = i;
			}
		}
		if (MatchCount == 1)
		{
			return RowNames[BestIdx];
		}
	}

	return NAME_None;
}

FText UEncyclopediaLibrary::BuildEncyclopediaText(const UDataTable* DataTable, FName RowName, AActor* FishActor)
{
	if (!DataTable)
	{
		return FText::FromString(TEXT("(도감 테이블 없음)"));
	}

	const FName Resolved = ResolveEncyclopediaRowName(DataTable, RowName, FishActor);
	if (Resolved.IsNone())
	{
		return FText::FromString(TEXT("(도감 데이터 없음)"));
	}

	const UScriptStruct* RowStruct = DataTable->GetRowStruct();
	const uint8* RowData = DataTable->FindRowUnchecked(Resolved);
	if (!RowStruct || !RowData)
	{
		return FText::FromString(TEXT("(도감 데이터 없음)"));
	}

	// 순서대로: 이름 → 스테이지 → 깊이
	FString NameStr, StageStr;
	ReadFieldAsString(RowStruct, RowData, TEXT("Name"), NameStr);
	ReadFieldAsString(RowStruct, RowData, TEXT("StageLevel"), StageStr);

	// 이름이 비어 있으면 행 이름이라도 보여준다.
	if (NameStr.IsEmpty())
	{
		NameStr = Resolved.ToString();
	}

	TArray<FString> Lines;
	Lines.Add(NameStr);
	if (!StageStr.IsEmpty())
	{
		Lines.Add(FString::Printf(TEXT("Level %s"), *TidyNumber(StageStr)));
	}
	// 수심: minDepth/maxDepth 가 있으면 그 중간값을 표시, 없으면 단일 Depth.
	const FString DepthDisplay = BuildDepthDisplay(RowStruct, RowData);
	if (!DepthDisplay.IsEmpty())
	{
		Lines.Add(FString::Printf(TEXT("Depth %sm"), *DepthDisplay));
	}

	return FText::FromString(FString::Join(Lines, TEXT("\n")));
}

bool UEncyclopediaLibrary::ShowEncyclopediaText(UTextRenderComponent* Target, const UDataTable* DataTable, FName RowName, UFont* KoreanFont, AActor* FishActor)
{
	if (!Target)
	{
		return false;
	}

	// 1) 한글 폰트 적용 — 이게 있어야 TextRender 에 한글이 보인다(기본 Roboto엔 한글 글리프가 없음).
	//    노드에 폰트를 꽂아주면 그걸 쓰고, 안 꽂았으면 "약속된 경로"에서 자동으로 찾아 적용한다.
	KoreanFont = ResolveKoreanFont(KoreanFont);
	if (KoreanFont)
	{
		Target->SetFont(KoreanFont);
	}

	// 2) 행을 찾아 "이름/스테이지/깊이" 텍스트로 표시
	const FText Text = BuildEncyclopediaText(DataTable, RowName, FishActor);
	Target->SetText(Text);

	// 행을 정상적으로 찾았는지 여부 반환
	const FName Resolved = ResolveEncyclopediaRowName(DataTable, RowName, FishActor);
	return !Resolved.IsNone();
}

bool UEncyclopediaLibrary::ShowEncyclopediaFields(UTextRenderComponent* NameText, UTextRenderComponent* StageText, UTextRenderComponent* DepthText, const UDataTable* DataTable, FName RowName, UFont* KoreanFont, AActor* FishActor)
{
	// 한글이 들어갈 수 있는 건 "이름"뿐이라, 한글 폰트는 이름 텍스트에만 적용한다.
	// Stage/Depth 는 "Level N"/"Depth Nm" ASCII 라 폰트를 건드리지 않는다(에디터에서 지정한 폰트 유지).
	KoreanFont = ResolveKoreanFont(KoreanFont);
	if (KoreanFont && NameText)
	{
		NameText->SetFont(KoreanFont);
	}

	const FName Resolved = DataTable ? ResolveEncyclopediaRowName(DataTable, RowName, FishActor) : NAME_None;
	const UScriptStruct* RowStruct = DataTable ? DataTable->GetRowStruct() : nullptr;
	const uint8* RowData = (DataTable && !Resolved.IsNone()) ? DataTable->FindRowUnchecked(Resolved) : nullptr;

	// 행을 못 찾으면 이름 칸에만 사유를 띄우고, 나머지는 비운다.
	if (!RowStruct || !RowData)
	{
		const FText Reason = FText::FromString(DataTable ? TEXT("(도감 데이터 없음)") : TEXT("(도감 테이블 없음)"));
		if (NameText)  { NameText->SetText(Reason); }
		if (StageText) { StageText->SetText(FText::GetEmpty()); }
		if (DepthText) { DepthText->SetText(FText::GetEmpty()); }
		return false;
	}

	// 이름 (없으면 행 이름으로 폴백)
	if (NameText)
	{
		FString NameStr;
		ReadFieldAsString(RowStruct, RowData, TEXT("Name"), NameStr);
		if (NameStr.IsEmpty()) { NameStr = Resolved.ToString(); }
		NameText->SetText(FText::FromString(NameStr));
	}

	// 스테이지 ("스테이지 N" — StageLevel 없으면 빈 텍스트)
	if (StageText)
	{
		FString StageStr;
		ReadFieldAsString(RowStruct, RowData, TEXT("StageLevel"), StageStr);
		StageText->SetText(StageStr.IsEmpty()
			? FText::GetEmpty()
			: FText::FromString(FString::Printf(TEXT("Level %s"), *TidyNumber(StageStr))));
	}

	// 깊이 ("깊이 Nm" — min/max 중간값 또는 단일 Depth, 없으면 빈 텍스트)
	if (DepthText)
	{
		const FString DepthDisplay = BuildDepthDisplay(RowStruct, RowData);
		DepthText->SetText(DepthDisplay.IsEmpty()
			? FText::GetEmpty()
			: FText::FromString(FString::Printf(TEXT("Depth %sm"), *DepthDisplay)));
	}

	return true;
}

void UEncyclopediaLibrary::FitTextRenderToWidth(UTextRenderComponent* Text, float MaxWidth, float MaxSize, float MinSize)
{
	if (!Text || MaxWidth <= 0.0f || MaxSize <= 0.0f)
	{
		return;
	}

	// 최대 크기로 둔 상태에서 실제 렌더 가로 폭을 잰다(GetTextLocalSize: Y=가로, Z=세로).
	Text->SetWorldSize(MaxSize);
	const float Width = Text->GetTextLocalSize().Y;

	// 폭을 넘으면 비례 축소(최소 MinSize). 측정이 0(폰트/등록 전)이면 그대로 MaxSize 유지.
	if (Width > MaxWidth)
	{
		const float Fitted = FMath::Max(MinSize, MaxSize * (MaxWidth / Width));
		Text->SetWorldSize(Fitted);
	}
}

// ── 자동 스폰용 행 읽기 헬퍼 ──────────────────────────────────────────────────
namespace
{
	// RowName 을 보정해 해당 행의 구조체/데이터 포인터를 얻는다.
	bool FindResolvedRow(const UDataTable* DataTable, FName RowName, const UScriptStruct*& OutStruct, const uint8*& OutData)
	{
		OutStruct = nullptr;
		OutData = nullptr;
		if (!DataTable)
		{
			return false;
		}
		FName Resolved = UEncyclopediaLibrary::ResolveEncyclopediaRowName(DataTable, RowName, nullptr);
		if (Resolved.IsNone())
		{
			Resolved = RowName; // 보정 실패 시 원래 이름으로라도 시도
		}
		OutStruct = DataTable->GetRowStruct();
		OutData = DataTable->FindRowUnchecked(Resolved);
		return OutStruct != nullptr && OutData != nullptr;
	}

	// 친근한 이름의 오브젝트 필드를 읽는다(하드/소프트 참조 모두). 소프트면 동기 로드.
	UObject* ReadObjectField(const UScriptStruct* Struct, const uint8* Data, const TCHAR* Friendly)
	{
		if (!Struct || !Data)
		{
			return nullptr;
		}
		for (TFieldIterator<FProperty> It(Struct); It; ++It)
		{
			FProperty* Prop = *It;
			if (!Struct->GetAuthoredNameForField(Prop).Equals(Friendly, ESearchCase::IgnoreCase))
			{
				continue;
			}
			const void* ValuePtr = Prop->ContainerPtrToValuePtr<void>(Data);
			if (const FSoftObjectProperty* SoftProp = CastField<FSoftObjectProperty>(Prop))
			{
				FSoftObjectPtr Soft = SoftProp->GetPropertyValue(ValuePtr);
				return Soft.LoadSynchronous();
			}
			if (const FObjectPropertyBase* ObjProp = CastField<FObjectPropertyBase>(Prop))
			{
				return ObjProp->GetObjectPropertyValue(ValuePtr);
			}
			break;
		}
		return nullptr;
	}

	// 액터에서 "FishRowName" 프로퍼티를 찾는다(BP 변수는 이름 그대로).
	FProperty* FindFishRowNameProp(AActor* Actor)
	{
		if (!Actor)
		{
			return nullptr;
		}
		return Actor->GetClass()->FindPropertyByName(FName(TEXT("FishRowName")));
	}
}

bool UEncyclopediaLibrary::GetRowDepthMeters(const UDataTable* DataTable, FName RowName, float& OutDepthMeters)
{
	OutDepthMeters = 0.0f;
	const UScriptStruct* Struct = nullptr;
	const uint8* Data = nullptr;
	if (!FindResolvedRow(DataTable, RowName, Struct, Data))
	{
		return false;
	}
	FString DepthStr;
	if (!ReadFieldAsString(Struct, Data, TEXT("Depth"), DepthStr) || DepthStr.IsEmpty())
	{
		return false;
	}
	OutDepthMeters = FCString::Atof(*DepthStr);
	return true;
}

bool UEncyclopediaLibrary::GetRowDepthRange(const UDataTable* DataTable, FName RowName, float& OutMinMeters, float& OutMaxMeters)
{
	OutMinMeters = 0.0f;
	OutMaxMeters = 0.0f;
	const UScriptStruct* Struct = nullptr;
	const uint8* Data = nullptr;
	if (!FindResolvedRow(DataTable, RowName, Struct, Data))
	{
		return false;
	}

	FString MinStr, MaxStr;
	const bool bHasMin = ReadFieldAsString(Struct, Data, TEXT("minDepth"), MinStr) && !MinStr.IsEmpty();
	const bool bHasMax = ReadFieldAsString(Struct, Data, TEXT("maxDepth"), MaxStr) && !MaxStr.IsEmpty();
	if (bHasMin || bHasMax)
	{
		float Mn = bHasMin ? FCString::Atof(*MinStr) : FCString::Atof(*MaxStr);
		float Mx = bHasMax ? FCString::Atof(*MaxStr) : FCString::Atof(*MinStr);
		if (Mn > Mx) { Swap(Mn, Mx); }
		OutMinMeters = Mn;
		OutMaxMeters = Mx;
		return true;
	}

	// 폴백: 단일 Depth 필드
	float Single = 0.0f;
	if (GetRowDepthMeters(DataTable, RowName, Single))
	{
		OutMinMeters = Single;
		OutMaxMeters = Single;
		return true;
	}
	return false;
}

bool UEncyclopediaLibrary::GetRowSpawnCount(const UDataTable* DataTable, FName RowName, int32& OutCount)
{
	OutCount = 0;
	const UScriptStruct* Struct = nullptr;
	const uint8* Data = nullptr;
	if (!FindResolvedRow(DataTable, RowName, Struct, Data))
	{
		return false;
	}
	FString CountStr;
	if (!ReadFieldAsString(Struct, Data, TEXT("SpawnCount"), CountStr) || CountStr.IsEmpty())
	{
		return false; // 구조체에 SpawnCount 필드가 없거나 비었음
	}
	OutCount = FCString::Atoi(*CountStr);
	return true;
}

bool UEncyclopediaLibrary::GetRowScanTime(const UDataTable* DataTable, FName RowName, float& OutScanTime)
{
	OutScanTime = 0.0f;
	const UScriptStruct* Struct = nullptr;
	const uint8* Data = nullptr;
	if (!FindResolvedRow(DataTable, RowName, Struct, Data))
	{
		return false;
	}
	FString ScanStr;
	if (!ReadFieldAsString(Struct, Data, TEXT("ScanTime"), ScanStr) || ScanStr.IsEmpty())
	{
		return false; // 구조체에 ScanTime 필드가 없거나 비었음
	}
	const float Parsed = FCString::Atof(*ScanStr);
	if (Parsed <= 0.0f)
	{
		return false; // 0 이하는 "값 없음"으로 보고 폴백시킨다
	}
	OutScanTime = Parsed;
	return true;
}

USkeletalMesh* UEncyclopediaLibrary::GetRowModelMesh(const UDataTable* DataTable, FName RowName)
{
	const UScriptStruct* Struct = nullptr;
	const uint8* Data = nullptr;
	if (!FindResolvedRow(DataTable, RowName, Struct, Data))
	{
		return nullptr;
	}
	return Cast<USkeletalMesh>(ReadObjectField(Struct, Data, TEXT("ModelMesh")));
}

UAnimSequenceBase* UEncyclopediaLibrary::GetRowAnimSequence(const UDataTable* DataTable, FName RowName)
{
	const UScriptStruct* Struct = nullptr;
	const uint8* Data = nullptr;
	if (!FindResolvedRow(DataTable, RowName, Struct, Data))
	{
		return nullptr;
	}

	// 1) 이름("AnimationSequence")으로 시도
	if (UAnimSequenceBase* Named = Cast<UAnimSequenceBase>(ReadObjectField(Struct, Data, TEXT("AnimationSequence"))))
	{
		return Named;
	}

	// 2) 폴백: 필드 이름이 어긋나도, 값이 AnimSequence 인 오브젝트 필드를 타입으로 찾는다.
	//    (구조체에 애니 필드는 하나뿐이라 안전. UserDefinedStruct 필드명이 재생성으로 바뀌어도 동작)
	for (TFieldIterator<FProperty> It(Struct); It; ++It)
	{
		FProperty* Prop = *It;
		const void* ValuePtr = Prop->ContainerPtrToValuePtr<void>(Data);
		UObject* Obj = nullptr;
		if (const FSoftObjectProperty* SoftProp = CastField<FSoftObjectProperty>(Prop))
		{
			Obj = SoftProp->GetPropertyValue(ValuePtr).LoadSynchronous();
		}
		else if (const FObjectPropertyBase* ObjProp = CastField<FObjectPropertyBase>(Prop))
		{
			Obj = ObjProp->GetObjectPropertyValue(ValuePtr);
		}
		if (UAnimSequenceBase* Anim = Cast<UAnimSequenceBase>(Obj))
		{
			return Anim;
		}
	}
	return nullptr;
}

FName UEncyclopediaLibrary::GetActorFishRowName(AActor* Actor)
{
	if (!Actor)
	{
		return NAME_None;
	}

	// 1) BP 변수 "FishRowName" 이 있으면 그걸 읽는다(종별 BP/BP_FishBase).
	if (FProperty* Prop = FindFishRowNameProp(Actor))
	{
		const void* ValuePtr = Prop->ContainerPtrToValuePtr<void>(Actor);
		if (const FNameProperty* NameProp = CastField<FNameProperty>(Prop))
		{
			const FName N = NameProp->GetPropertyValue(ValuePtr);
			if (!N.IsNone()) { return N; }
		}
		else if (const FStrProperty* StrProp = CastField<FStrProperty>(Prop))
		{
			const FString S = StrProp->GetPropertyValue(ValuePtr);
			if (!S.IsEmpty()) { return FName(*S); }
		}
		else if (const FTextProperty* TextProp = CastField<FTextProperty>(Prop))
		{
			const FString S = TextProp->GetPropertyValue(ValuePtr).ToString();
			if (!S.IsEmpty()) { return FName(*S); }
		}
	}

	// 2) 폴백: "FishRow=종이름" 태그에서 읽는다(변수가 없는 제네릭 물고기 = plain AActor).
	for (const FName& Tag : Actor->Tags)
	{
		const FString S = Tag.ToString();
		if (S.StartsWith(TEXT("FishRow=")))
		{
			const FString RowStr = S.RightChop(8); // "FishRow=" 길이 8
			return RowStr.IsEmpty() ? NAME_None : FName(*RowStr);
		}
	}
	return NAME_None;
}

bool UEncyclopediaLibrary::SetActorFishRowName(AActor* Actor, FName RowName)
{
	if (!Actor)
	{
		return false;
	}

	// 제네릭 물고기(FishRowName 변수가 없는 plain AActor)도 종을 알 수 있게 태그로도 각인.
	// (GetActorFishRowName 의 폴백이 이 태그를 읽는다 → 스캔 수집/도감 등록이 된다)
	if (!RowName.IsNone())
	{
		Actor->Tags.AddUnique(FName(*FString::Printf(TEXT("FishRow=%s"), *RowName.ToString())));
	}

	FProperty* Prop = FindFishRowNameProp(Actor);
	if (!Prop)
	{
		return false; // 변수는 없지만 위에서 태그로 각인했으니 등록은 가능
	}
	void* ValuePtr = Prop->ContainerPtrToValuePtr<void>(Actor);
	if (FNameProperty* NameProp = CastField<FNameProperty>(Prop))
	{
		NameProp->SetPropertyValue(ValuePtr, RowName);
		return true;
	}
	if (FStrProperty* StrProp = CastField<FStrProperty>(Prop))
	{
		StrProp->SetPropertyValue(ValuePtr, RowName.ToString());
		return true;
	}
	if (FTextProperty* TextProp = CastField<FTextProperty>(Prop))
	{
		TextProp->SetPropertyValue(ValuePtr, FText::FromName(RowName));
		return true;
	}
	return false;
}

bool UEncyclopediaLibrary::CheckWallAhead(AActor* Fish, float CheckDistance, float CheckRadius, float& OutAvoidYaw)
{
	OutAvoidYaw = 0.0f;
	if (!Fish)
	{
		return false;
	}
	UWorld* World = Fish->GetWorld();
	if (!World)
	{
		return false;
	}

	const FVector Start = Fish->GetActorLocation();
	const FVector Forward = Fish->GetActorForwardVector();
	const FVector End = Start + Forward * CheckDistance;

	FHitResult Hit;
	FCollisionQueryParams Params(SCENE_QUERY_STAT(FishWallAhead), false, Fish);
	const bool bHit = World->SweepSingleByChannel(
		Hit, Start, End, FQuat::Identity, ECC_WorldStatic,
		FCollisionShape::MakeSphere(FMath::Max(1.0f, CheckRadius)), Params);

	if (!bHit)
	{
		return false; // 앞이 비었음
	}

	// 벽 표면 법선의 수평 성분 방향(=벽에서 멀어지는 쪽)으로 피할 Yaw 를 잡는다.
	FVector Normal = Hit.ImpactNormal;
	Normal.Z = 0.0f;
	if (Normal.IsNearlyZero())
	{
		// 정면 벽(법선이 수직 등) → 그냥 반대로 돈다.
		OutAvoidYaw = Fish->GetActorRotation().Yaw + 180.0f;
	}
	else
	{
		Normal.Normalize();
		OutAvoidYaw = Normal.Rotation().Yaw;
	}
	return true;
}
