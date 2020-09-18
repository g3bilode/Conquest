// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitSpawnerWidget.h"
#include "../PlayerCharacter/ConquestPlayerController.h"


void UUnitSpawnerWidget::OnSlotSelected()
{
	AConquestPlayerController* conquestPlayerController = GetWorld()->GetFirstPlayerController<AConquestPlayerController>();
	conquestPlayerController->EnableSpawningMode(UnitClass);
}


void UUnitSpawnerWidget::NativeConstruct()
{
	Super::NativeConstruct();
	Populate();
}
