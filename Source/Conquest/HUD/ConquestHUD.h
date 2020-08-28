// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ConquestHUD.generated.h"

/**
 * 
 */
UCLASS()
class CONQUEST_API AConquestHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AConquestHUD();

	void OnBarrackSelection(TArray<class AUnitSlot*> UnitSlots, int32 LaneIndex);

	void OnEmptyUnitSlotSelection(int32 LaneIndex, int32 SlotIndex);

	void OnOccupiedUnitSlotSelection();

protected:
	void BeginPlay() override;

public:
	UPROPERTY()
	TSubclassOf<UUserWidget> GameHUDWidgetClass;

	UPROPERTY()
	class UGameplayWidget* GameHUDWidget;

};
