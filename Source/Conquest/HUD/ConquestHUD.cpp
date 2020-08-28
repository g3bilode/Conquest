// Fill out your copyright notice in the Description page of Project Settings.


#include "ConquestHUD.h"
#include "Blueprint/UserWidget.h"
#include "BarracksWidget.h"
#include "GameplayWidget.h"
#include "UnitActionsWidget.h"

AConquestHUD::AConquestHUD()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> conquestHud(TEXT("/Game/Conquest/UI/HUD/Gameplay_HUD"));
	GameHUDWidgetClass = conquestHud.Class;
}


void AConquestHUD::OnBarrackSelection(TArray<class AUnitSlot*> UnitSlots, int32 LaneIndex)
{
	GameHUDWidget->BarrackWidget->PopulateGridData(UnitSlots, LaneIndex);
}


void AConquestHUD::OnEmptyUnitSlotSelection(int32 LaneIndex, int32 SlotIndex)
{
	GameHUDWidget->UnitActionsWidget->PopulateSpawnerGridData(LaneIndex, SlotIndex);
}

void AConquestHUD::OnOccupiedUnitSlotSelection()
{
	GameHUDWidget->UnitActionsWidget->PopulateUpgradeGridData();
}

void AConquestHUD::BeginPlay()
{
	Super::BeginPlay();

	// Setup Game HUD
	GameHUDWidget = CreateWidget<UGameplayWidget>(GetWorld(), GameHUDWidgetClass, "Game HUD");
	GameHUDWidget->AddToViewport();

}
