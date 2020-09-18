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

protected:
	void BeginPlay() override;

public:	
	/* Update unit spawner location. */
	void UpdatePosition(APlayerController* LocalController);

	/* Attempt to purchase this spawner. */
	bool AttemptPurchase();

	/* ConquestUnit spawned by this spawner. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Unit")
	TSubclassOf<class AConquestUnit> UnitClass;

	// TODO: Remove this and use Unit's?
	/* Icon for this unit spawner. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "UI")
	UTexture2D* Icon;

	/* Purchase cost for this unit spawner. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Cost")
	int32 PurchaseCost;

	/* Team index. */
	int32 TeamIndex;

private:
	/* Currently selected barracks. */
	class ABarracks* ActiveBarracks;

	/* Lane index for spawner's owning barracks. */
	int32 LaneIndex;

	/* LaneDestinations for spawner's owning barracks. */
	TArray<FVector> LaneDestinations;

	/* Respond to combat phase begin delegate. */
	UFUNCTION()
	void RespondToCombatPhaseBegin();

};
