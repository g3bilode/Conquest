// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitSpawnerWidget.h"
#include "../PlayerCharacter/ConquestPlayerController.h"


void UUnitSpawnerWidget::EnableSpawner(int32 LaneIndex, int32 SlotIndex)
{
	SetVisibility(ESlateVisibility::Visible);
	_LaneIndex = LaneIndex;
	_SlotIndex = SlotIndex;
}


void UUnitSpawnerWidget::DisableSpawner()
{
	SetVisibility(ESlateVisibility::Hidden);
}

void UUnitSpawnerWidget::OnSlotSelected()
{
	AConquestPlayerController* conquestPlayerController = GetWorld()->GetFirstPlayerController<AConquestPlayerController>();
	conquestPlayerController->AttemptSpawnUnit(UnitClass, _LaneIndex, _SlotIndex);
}


void UUnitSpawnerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Populate();
	SetVisibility(ESlateVisibility::Hidden);
}
