// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitUpgradeWidget.h"
#include "../Units/Spawners/UnitSpawner.h"


void UUnitUpgradeWidget::Populate(class AUnitSpawner* InUnitSpawner, int32 InEvolutionIndex)
{
	UnitSpawner = InUnitSpawner;
	EvolutionIndex = InEvolutionIndex;
	GenerateUI(UnitSpawner->GetUnitEvolutions()[EvolutionIndex]);
}


void UUnitUpgradeWidget::ClearData()
{
	UnitSpawner = nullptr;
	EvolutionIndex = -1;
	GenerateUI(nullptr);
}


void UUnitUpgradeWidget::OnSlotSelected()
{
	// tell spawner to upgrade
	if (IsValid(UnitSpawner))
	{
		UnitSpawner->AttemptEvolve(EvolutionIndex);
	}
}


void UUnitUpgradeWidget::NativeConstruct()
{
	Super::NativeConstruct();
}
