// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitActionsWidget.h"
#include "UnitSpawnerWidget.h"
#include "UnitUpgradeWidget.h"


void UUnitActionsWidget::PopulateSpawnerGridData(int32 LaneIndex)
{
	for (UUnitSpawnerWidget* unitSpawnerWidget : UnitSpawnerGrid)
	{
		unitSpawnerWidget->EnableSpawner(LaneIndex);
	}
	for (UUnitUpgradeWidget* unitUpgradeWidget : UnitUpgradeGrid)
	{
		unitUpgradeWidget->DisableUpgrade();
	}
}


void UUnitActionsWidget::PopulateUpgradeGridData()
{
	for (UUnitUpgradeWidget* unitUpgradeWidget : UnitUpgradeGrid)
	{
		unitUpgradeWidget->EnableUpgrade();
	}
	for (UUnitSpawnerWidget* unitSpawnerWidget : UnitSpawnerGrid)
	{
		unitSpawnerWidget->DisableSpawner();
	}
}
