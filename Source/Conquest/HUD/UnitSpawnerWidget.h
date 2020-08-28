// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnitSpawnerWidget.generated.h"

/**
 * 
 */
UCLASS()
class CONQUEST_API UUnitSpawnerWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	/* Populate the slot data. */
	UFUNCTION(BlueprintImplementableEvent)
	void Populate();

	/* Enable this spawner for given lane. */
	void EnableSpawner(int32 LaneIndex, int32 SlotIndex);

	/* Disable this spawner. */
	void DisableSpawner();

	/* Event on button click. */
	UFUNCTION(BlueprintCallable)
	void OnSlotSelected();

	/* Unit to spawn. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UnitSpawner")
	TSubclassOf<class AConquestUnit> UnitClass;

protected:
	void NativeConstruct() override;

private:
	/* Lane index. */
	int32 _LaneIndex;
	/* Unit slot index. */
	int32 _SlotIndex;
};
