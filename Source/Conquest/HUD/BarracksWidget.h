// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "BarracksWidget.generated.h"

/**
 * 
 */
UCLASS()
class CONQUEST_API UBarracksWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/* Populate the UnitSlotGrid UI. */
	void PopulateGridData(TArray<class AUnitSlot*> UnitSlots);

	/* Grid of unit slot buttons for selected barrack. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UnitSlots")
	TArray<class UUnitSlotWidget*> UnitSlotGrid;
};
