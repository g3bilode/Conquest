// Fill out your copyright notice in the Description page of Project Settings.


#include "BarracksWidget.h"
#include "../Barracks/UnitSlot.h"
#include "../Units/ConquestUnit.h"
#include "UnitSlotWidget.h"


void UBarracksWidget::PopulateGridData(TArray<AUnitSlot*> UnitSlots)
{
	for (int32 i=0;  i < UnitSlots.Num(); i++)
	{
		AUnitSlot* unitSlot = UnitSlots[i];
		UUnitSlotWidget* unitSlotWidget = UnitSlotGrid[i];
		if (unitSlot->IsOccupied())
		{
			UTexture2D* unitIcon = unitSlot->GetOccupyingUnit()->Icon;
			unitSlotWidget->SetIcon(unitIcon);
		}
		else
		{
			unitSlotWidget->SetIcon(unitSlotWidget->DefaultUnitSlotIcon);
		}
	}
}
