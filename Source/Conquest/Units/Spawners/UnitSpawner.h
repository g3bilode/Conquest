// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../../Interfaces/ConquestSelectableInterface.h"
#include "UnitSpawner.generated.h"

UCLASS()
class CONQUEST_API AUnitSpawner : public AActor, public IConquestSelectableInterface
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

	/* Attempt to evolve unit to class at given index. */
	bool AttemptEvolve(int32 EvolutionIndex);

	/* Get current unit evolution classes. */
	TArray<TSubclassOf<class AConquestUnit>> GetUnitEvolutions();

	/* ConquestUnit spawned by this spawner. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Unit")
	TSubclassOf<class AConquestUnit> UnitClass;

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

	/* Is current spawner active? */
	bool bIsActive;

	/* Lane index for spawner's owning barracks. */
	int32 LaneIndex;

	/* LaneDestinations for spawner's owning barracks. */
	TArray<FVector> LaneDestinations;

	/* Local player controller. */
	class AConquestPlayerController* LocalPlayerController;

	/* Respond to combat phase begin delegate. */
	UFUNCTION()
	void RespondToCombatPhaseBegin();


	bool OnSelectionGained_Implementation(AConquestPlayerController* initiator) override;

	bool OnSelectionChanged_Implementation(AConquestPlayerController* initiator, AActor* NewSelection) override;

};
