// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UnitSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class CONQUEST_API UUnitSlotWidget : public UUserWidget
{
	GENERATED_BODY()
public:


public:
	/* Event on button click. */
	UFUNCTION(BlueprintCallable)
	void OnSlotSelected();

	/* Populate with unit slot data. */
	void Populate(class AUnitSlot* UnitSlot, int32 LaneIndex, int32 SlotIndex);

	/* Set the unit slot button icon. */
	UFUNCTION(BlueprintImplementableEvent)
	void SetIcon(UTexture2D* NewIcon);

	/* Icon for empty unit slot. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* DefaultUnitSlotIcon;


private:
	/* Corresponding UnitSlot actor. */
	UPROPERTY()
	AUnitSlot* _UnitSlot;
	/* Lane index. */
	int32 _LaneIndex;
	/* Slot index. */
	int32 _SlotIndex;
	/* ConquestHUD reference. */
	UPROPERTY()
	class AConquestHUD* _ConquestHUD;

	/* Returns reference to ConquestHUD. */
	class AConquestHUD* GetConquestHUD();

protected:
	void NativeConstruct() override;

};
