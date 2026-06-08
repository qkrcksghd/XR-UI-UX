// Fill out your copyright notice in the Description page of Project Settings.

#include "Menu/MenuGameMode.h"
#include "Menu/MenuPawn.h"
#include "UObject/ConstructorHelpers.h"

AMenuGameMode::AMenuGameMode()
{
	// 기본값 = C++ 메뉴 폰. 아래에서 BP_MenuPawn 이 있으면 그걸로 덮어쓴다.
	DefaultPawnClass = AMenuPawn::StaticClass();

	// 기본 폰 = BP_MenuPawn (디테일에서 IMC/IA/레이저 설정을 지정한 BP 자식).
	// 경로가 맞으면 자동 지정되어 메뉴 레벨 Play 시 이 폰이 스폰된다.
	// (BP_MenuPawn 을 옮기거나 이름 바꾸면 이 경로도 같이 고쳐야 함)
	static ConstructorHelpers::FClassFinder<APawn> MenuPawnBP(TEXT("/Game/CH/Menu/BP_MenuPawn"));
	if (MenuPawnBP.Succeeded())
	{
		DefaultPawnClass = MenuPawnBP.Class;
	}
}
