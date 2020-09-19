// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnitUpgradeWidget.generated.h"

/**
 * 
 */
UCLASS()
class CONQUEST_API UUnitUpgradeWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	// TODO: Create common parent with UnitSpawnedWidget

	/* Populate widget data with selected spawner info. */
	void Populate(class AUnitSpawner* InUnitSpawner, int32 SlotIndex);

	/* Clear the widget data. */
	void ClearData();

	/* Populate the slot data. */
	UFUNCTION(BlueprintImplementableEvent)
	void GenerateUI(TSubclassOf<class AConquestUnit> UnitClass);

	/* Event on button click. */
	UFUNCTION(BlueprintCallable)
	void OnSlotSelected();

protected:
	void NativeConstruct() override;

private:

	/* Unit spawner reference. */
	class AUnitSpawner* UnitSpawner;

	/* Evolution Index */
	int32 EvolutionIndex;
};
