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
