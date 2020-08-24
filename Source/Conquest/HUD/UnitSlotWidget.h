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
	/* Set the unit slot button icon. */
	UFUNCTION(BlueprintImplementableEvent)
	void SetIcon(UTexture2D* NewIcon);

	/* Icon for empty unit slot. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere)
	UTexture2D* DefaultUnitSlotIcon;
	
};
