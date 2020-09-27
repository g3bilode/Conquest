// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "ResourceDripWidget.generated.h"

/**
 * 
 */
UCLASS()
class CONQUEST_API UResourceDripWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	/* Helper to setup widget properties. */
	void Setup(const FLinearColor InTextColour);

	/* Draw drip. */
	UFUNCTION(BlueprintImplementableEvent)
	void DrawDrip(int32 amount);

	/* Text colour. */
	UPROPERTY(BlueprintReadOnly)
	FLinearColor TextColour;
	
};
