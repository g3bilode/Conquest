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
	/* Enable this upgrade. */
	void EnableUpgrade();

	/* Disable */
	void DisableUpgrade();

protected:
	void NativeConstruct() override;
};
