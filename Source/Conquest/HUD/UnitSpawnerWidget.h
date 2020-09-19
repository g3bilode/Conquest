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

	/* Event on button click. */
	UFUNCTION(BlueprintCallable)
	void OnSlotSelected();

	/* Unit to spawn. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UnitSpawner")
	TSubclassOf<class AUnitSpawner> UnitClass;

protected:
	void NativeConstruct() override;

};
