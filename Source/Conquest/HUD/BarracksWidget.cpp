// Fill out your copyright notice in the Description page of Project Settings.


#include "BarracksWidget.h"
#include "../Barracks/UnitSlot.h"
#include "UnitSlotWidget.h"


void UBarracksWidget::PopulateGridData(TArray<AUnitSlot*> UnitSlots, int32 LaneIndex)
{
	for (int32 i=0;  i < UnitSlots.Num(); i++)
	{
		AUnitSlot* unitSlot = UnitSlots[i];

		if (ensure(UnitSlotGrid.IsValidIndex(i)))
		{
			UUnitSlotWidget* unitSlotWidget = UnitSlotGrid[i];
			unitSlotWidget->Populate(unitSlot, LaneIndex, i);
		}
		unitSlot->UnitSlot_OnUpdate.Clear();
		unitSlot->UnitSlot_OnUpdate.BindDynamic(this, &UBarracksWidget::RefreshGrid);
	}
}

void UBarracksWidget::RefreshGrid()
{
	for (UUnitSlotWidget* unitSlotWidget : UnitSlotGrid)
	{
		unitSlotWidget->RefreshDisplay();
	}
}
