// Fill out your copyright notice in the Description page of Project Settings.


#include "ConquestHUD.h"
#include "Blueprint/UserWidget.h"
#include "GameplayWidget.h"
#include "UnitActionsWidget.h"


AConquestHUD::AConquestHUD()
{
	static ConstructorHelpers::FClassFinder<UUserWidget> conquestHud(TEXT("/Game/Conquest/UI/HUD/Gameplay_HUD"));
	GameHUDWidgetClass = conquestHud.Class;
}


void AConquestHUD::OnSpawnerSelected(class AUnitSpawner* SelectedSpawner)
{
	GameHUDWidget->UnitActionsWidget->PopulateUpgradeGridData(SelectedSpawner);
}


void AConquestHUD::OnSpawnerDeselected()
{
	GameHUDWidget->UnitActionsWidget->ClearUpgradeGridData();
}


void AConquestHUD::ConstructHUD()
{
	// Setup Game HUD
	GameHUDWidget = CreateWidget<UGameplayWidget>(GetWorld(), GameHUDWidgetClass, "Game HUD");
	GameHUDWidget->AddToViewport();
}
