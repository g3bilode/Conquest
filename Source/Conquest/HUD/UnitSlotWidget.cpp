// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitSlotWidget.h"
#include "../Conquest.h"
#include "../Barracks/UnitSlot.h"
#include "../PlayerCharacter/ConquestPlayerController.h"
#include "../Units/ConquestUnit.h"
#include "ConquestHUD.h"


void UUnitSlotWidget::OnSlotSelected()
{
	if (_UnitSlot->IsOccupied())
	{
		GetConquestHUD()->OnOccupiedUnitSlotSelection();
	}
	else
	{
		GetConquestHUD()->OnEmptyUnitSlotSelection(_LaneIndex, _SlotIndex);
	}
}


void UUnitSlotWidget::Populate(class AUnitSlot* UnitSlot, int32 LaneIndex, int32 SlotIndex)
{
	_UnitSlot = UnitSlot;
	_LaneIndex = LaneIndex;
	_SlotIndex = SlotIndex;
	RefreshDisplay();
	SetVisibility(ESlateVisibility::Visible);
}


void UUnitSlotWidget::RefreshDisplay()
{
	if (IsValid(_UnitSlot))
	{
		if (_UnitSlot->IsOccupied())
		{
			AConquestUnit* occupyingUnit = _UnitSlot->GetOccupyingUnit();
			if (IsValid(occupyingUnit))
			{
				UTexture2D* unitIcon = occupyingUnit->Icon;
				SetIcon(unitIcon);
			}
		}
		else
		{
			SetIcon(DefaultUnitSlotIcon);
		}
	}
}


class AConquestHUD* UUnitSlotWidget::GetConquestHUD()
{
	if (!IsValid(_ConquestHUD))
	{
		AConquestPlayerController* conquestPlayerController = GetWorld()->GetFirstPlayerController<AConquestPlayerController>();
		_ConquestHUD = conquestPlayerController->GetHUD<AConquestHUD>();
	}
	return _ConquestHUD;
}


void UUnitSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetVisibility(ESlateVisibility::Hidden);
}
