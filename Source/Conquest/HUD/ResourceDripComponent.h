// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/WidgetComponent.h"
#include "ResourceDripComponent.generated.h"

/**
 * 
 */
UCLASS()
class CONQUEST_API UResourceDripComponent : public UWidgetComponent
{
	GENERATED_BODY()
	
public:
	UResourceDripComponent();

	/* Draw an instance of resource drip. */
	void ActivateDripWidget(int32 amount);

	/* Setup widget attributes. */
	void SetupWidget(FLinearColor TextColour);

};
