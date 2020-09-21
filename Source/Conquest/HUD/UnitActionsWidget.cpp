// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitActionsWidget.h"
#include "UnitUpgradeWidget.h"
#include "../Units/Spawners/UnitSpawner.h"
#include "../Units/ConquestUnit.h"


void UUnitActionsWidget::PopulateUpgradeGridData(AUnitSpawner* UnitSpawner)
{
	int32 evolutionNum = UnitSpawner->GetUnitEvolutions().Num();
	int32 slotNum = 0;
	for (UUnitUpgradeWidget* unitUpgradeWidget : UnitUpgradeGrid)
	{
		if (slotNum < evolutionNum)
		{
			unitUpgradeWidget->Populate(UnitSpawner, slotNum);
		}
		else
		{
			unitUpgradeWidget->ClearData();
		}
		slotNum++;
	}
}


void UUnitActionsWidget::ClearUpgradeGridData()
{
	for (UUnitUpgradeWidget* unitUpgradeWidget : UnitUpgradeGrid)
	{
		unitUpgradeWidget->ClearData();
	}
}
