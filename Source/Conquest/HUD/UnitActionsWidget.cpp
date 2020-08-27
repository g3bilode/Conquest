// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitActionsWidget.h"
#include "UnitSpawnerWidget.h"

void UUnitActionsWidget::PopulateGridData(int32 LaneIndex)
{
	for (UUnitSpawnerWidget* unitSpawnerWidget : UnitSpawnerGrid)
	{
		unitSpawnerWidget->EnableSpawner(LaneIndex);
	}
}
