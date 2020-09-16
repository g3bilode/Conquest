// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnitSpawner.generated.h"

UCLASS()
class CONQUEST_API AUnitSpawner : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AUnitSpawner();

public:	
	/* Update unit spawner location. */
	void UpdatePosition(APlayerController* LocalController);

	/* Attempt to purchase this spawner. */
	void AttemptPurchase();

	/* Icon for this unit spawner. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "UI")
	UTexture2D* Icon;

private:
	/* Currently selected barracks. */
	class ABarracks* ActiveBarracks;

};
