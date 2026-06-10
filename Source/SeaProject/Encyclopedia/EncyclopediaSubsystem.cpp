// Fill out your copyright notice in the Description page of Project Settings.

#include "EncyclopediaSubsystem.h"
#include "EncyclopediaLibrary.h"

void UEncyclopediaSubsystem::MarkCollected(FName RowName)
{
	const FName Key = UEncyclopediaLibrary::ToCanonicalKey(RowName);
	if (!Key.IsNone())
	{
		CollectedKeys.Add(Key);
	}
}

bool UEncyclopediaSubsystem::IsCollected(FName RowName) const
{
	const FName Key = UEncyclopediaLibrary::ToCanonicalKey(RowName);
	return !Key.IsNone() && CollectedKeys.Contains(Key);
}

void UEncyclopediaSubsystem::ResetCollected()
{
	CollectedKeys.Empty();
}

int32 UEncyclopediaSubsystem::GetCollectedCount() const
{
	return CollectedKeys.Num();
}

void UEncyclopediaSubsystem::MarkChestCollected(FName ChestKey)
{
	// 상자 키는 표준화하지 않고 그대로 저장(번호가 잘려 합쳐지는 것 방지).
	if (!ChestKey.IsNone())
	{
		CollectedChestKeys.Add(ChestKey);
	}
}

bool UEncyclopediaSubsystem::IsChestCollected(FName ChestKey) const
{
	return !ChestKey.IsNone() && CollectedChestKeys.Contains(ChestKey);
}

int32 UEncyclopediaSubsystem::GetChestCollectedCount() const
{
	return CollectedChestKeys.Num();
}
