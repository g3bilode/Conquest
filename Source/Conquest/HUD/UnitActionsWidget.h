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
	/* Populate the UnitSlotGrid UI. */
	void PopulateGridData();

	/* Grid of unit slot buttons for selected barrack. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UnitSpawners")
	TArray<class UUnitSpawnerWidget*> UnitSpawnerGrid;
};
