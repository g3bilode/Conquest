// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayWidget.generated.h"

/**
 * 
 */
UCLASS()
class CONQUEST_API UGameplayWidget : public UUserWidget
{
	GENERATED_BODY()
	
public:
	UPROPERTY(BlueprintReadWrite)
	class UBarracksWidget* BarrackWidget;

	UPROPERTY(BlueprintReadWrite)
	class UUnitActionsWidget* UnitActionsWidget;
};
