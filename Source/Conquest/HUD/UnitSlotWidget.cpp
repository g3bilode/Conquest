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
		GetConquestHUD()->OnEmptyUnitSlotSelection(_LaneIndex);
	}
}


void UUnitSlotWidget::Populate(class AUnitSlot* UnitSlot, int32 LaneIndex)
{
	_UnitSlot = UnitSlot;
	_LaneIndex = LaneIndex;
	if (_UnitSlot->IsOccupied())
	{
		UTexture2D* unitIcon = _UnitSlot->GetOccupyingUnit()->Icon;
		SetIcon(unitIcon);
	}
	else
	{
		SetIcon(DefaultUnitSlotIcon);
	}
	SetVisibility(ESlateVisibility::Visible);
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
