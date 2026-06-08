// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Pawn.h"
#include "InputActionValue.h"
#include "MenuPawn.generated.h"

class USceneComponent;
class UCameraComponent;
class UMotionControllerComponent;
class UWidgetComponent;
class UWidgetInteractionComponent;
class UStaticMeshComponent;
class UInputMappingContext;
class UInputAction;

// 시작 화면(메인 메뉴) 전용 VR 폰.
// UI는 평평한 2D 메뉴판이지만, VR에서 올바르게 보이도록 화면(viewport)이 아니라
// World 위젯 컴포넌트로 "카메라 정면 일정 거리"에 띄운다(부드럽게 따라옴).
// 오른손 컨트롤러에서 가는 레이저 + 끝점 도트를 쏘아 버튼을 가리키고, 트리거로 클릭한다.
//
// [에디터 작업]
//  - 이 폰을 부모로 BP_MenuPawn 생성 → 컴포넌트에서 MenuPanel 선택 →
//    Details > User Interface > Widget Class = WBP_MainMenu 지정.
//  - Class Defaults 에서 MenuMappingContext = IMC_VR, ClickAction = 오른손 트리거 IA.
UCLASS()
class SEAPROJECT_API AMenuPawn : public APawn
{
	GENERATED_BODY()

public:
	AMenuPawn();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	// ── 컴포넌트 ─────────────────────────────────────────────────────────
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu | Components")
	USceneComponent* VRRoot;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu | Components")
	UCameraComponent* Camera;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu | Components")
	UMotionControllerComponent* LeftHandController;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu | Components")
	UMotionControllerComponent* RightHandController;

	// 2D 메뉴판(WBP_MainMenu). 매 틱 카메라 정면으로 부드럽게 따라오게 한다.
	// BP_MenuPawn 에서 이 컴포넌트의 Widget Class = WBP_MainMenu 로 지정.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu | Components")
	UWidgetComponent* MenuPanel;

	// 오른손 레이저. 패널을 가리키면 hover, 트리거로 PressPointerKey 클릭.
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu | Components")
	UWidgetInteractionComponent* WidgetInteraction;

	// 레이저 빔(가는 실린더)과 끝점 도트(작은 구).
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu | Components")
	UStaticMeshComponent* LaserBeam;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Menu | Components")
	UStaticMeshComponent* LaserDot;

	// ── 입력 (Enhanced Input) ────────────────────────────────────────────
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Menu | Input")
	UInputMappingContext* MenuMappingContext;

	// 클릭(오른손 트리거) 입력 액션.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Menu | Input")
	UInputAction* ClickAction;

	// ── 패널 배치 ────────────────────────────────────────────────────────
	// 카메라 정면으로 이만큼(cm) 떨어진 곳에 패널을 둔다.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu | Panel")
	float PanelDistance = 250.0f;

	// 패널을 눈높이에서 위/아래로 보정(cm). 음수면 아래로.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu | Panel")
	float PanelVerticalOffset = -20.0f;

	// 패널이 카메라를 따라오는 속도(위치/회전). 클수록 즉각, 작을수록 느긋하게 따라옴.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu | Panel")
	float FollowSpeed = 6.0f;

	// 글자가 좌우로 뒤집혀 보이면 체크(패널 앞뒤 면을 뒤집는다).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu | Panel")
	bool bFlipPanelFacing = false;

	// ── 레이저 설정 ──────────────────────────────────────────────────────
	// 레이저 최대 사거리(cm).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu | Laser")
	float PointerDistance = 3000.0f;

	// 컨트롤러 정면과 레이저가 어긋날 때 보정하는 회전(손전등 조준 오프셋과 같은 개념).
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu | Laser")
	FRotator PointerAimOffset = FRotator::ZeroRotator;

	// 레이저 빔/도트를 보일지.
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Menu | Laser")
	bool bShowLaser = true;

private:
	// 트리거 당김/뗌 → 위젯 좌클릭 누름/뗌
	void Click_Pressed();
	void Click_Released();

	// 패널을 카메라 정면으로 부드럽게 이동/회전(매 틱).
	void UpdatePanelTransform(float DeltaTime);

	// 레이저 빔 길이와 끝점 도트 위치 갱신(매 틱).
	void UpdateLaser();

	// 패널을 첫 틱에 곧바로 목표 위치로 스냅(처음에 날아오는 것 방지).
	bool bPanelInitialized = false;
};
