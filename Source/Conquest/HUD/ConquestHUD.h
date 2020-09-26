// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ConquestHUD.generated.h"

/**
 * 
 */
UCLASS()
class CONQUEST_API AConquestHUD : public AHUD
{
	GENERATED_BODY()
	
public:
	AConquestHUD();

	/* Event on spawner selected. */
	void OnSpawnerSelected(class AUnitSpawner* SelectedSpawner);

	/* Event on spawner deselected. */
	void OnSpawnerDeselected();

	/* Construct the HUD widget. */
	void ConstructHUD();


public:
	/* HUD widget class. */
	UPROPERTY()
	TSubclassOf<UUserWidget> GameHUDWidgetClass;

	/* HUD widget reference. */
	UPROPERTY()
	class UGameplayWidget* GameHUDWidget;

};
