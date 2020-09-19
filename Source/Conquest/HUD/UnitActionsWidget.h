// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnitActionsWidget.generated.h"

/**
 * 
 */
UCLASS()
class CONQUEST_API UUnitActionsWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:

	/* Populate the UnitUpgradeGrid UI. */
	void PopulateUpgradeGridData(class AUnitSpawner* UnitSpawner);

	/* Grid of unit upgrade buttons for selected barrack. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Unit Upgrades")
	TArray<class UUnitUpgradeWidget*> UnitUpgradeGrid;
};
