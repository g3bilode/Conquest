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
		UE_LOG(LogConquest, Log, TEXT("Upgrade time!"));
	}
	else
	{
		AConquestPlayerController* conquestPlayerController = GetWorld()->GetFirstPlayerController<AConquestPlayerController>();
		AConquestHUD* conquestHUD = conquestPlayerController->GetHUD<AConquestHUD>();
		conquestHUD->OnEmptyUnitSlotSelection(_LaneIndex);
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


void UUnitSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	SetVisibility(ESlateVisibility::Hidden);
}
