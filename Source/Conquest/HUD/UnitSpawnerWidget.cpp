// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitSpawnerWidget.h"
#include "../PlayerCharacter/ConquestPlayerController.h"


void UUnitSpawnerWidget::EnableSpawner(int32 LaneIndex)
{
	SetVisibility(ESlateVisibility::Visible);
	_LaneIndex = LaneIndex;
}


void UUnitSpawnerWidget::OnSlotSelected()
{
	AConquestPlayerController* conquestPlayerController = GetWorld()->GetFirstPlayerController<AConquestPlayerController>();
	conquestPlayerController->AttemptSpawnUnit(UnitClass, _LaneIndex);
}


void UUnitSpawnerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Populate();
	SetVisibility(ESlateVisibility::Hidden);
}
