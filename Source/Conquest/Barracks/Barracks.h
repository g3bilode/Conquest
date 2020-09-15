// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interfaces/ConquestSelectableInterface.h"
#include "Barracks.generated.h"

UCLASS()
class CONQUEST_API ABarracks : public AActor, public IConquestSelectableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABarracks();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


public:

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "UnitSlots")
	TArray<class AUnitSlot*> UnitSlots;

	UPROPERTY(EditAnywhere, Category = "Team")
	int32 TeamIndex;

	UPROPERTY(EditAnywhere, Category = "Team")
	int32 LaneIndex;

	class AUnitSlot* GetFreeUnitSlot();

	class AUnitSlot* GetUnitSlotAtIndex(int32 SlotIndex);

	/* Return location of the nearest slot, if free. If occupied, returns null. */
	FVector GetNearestFreeSlotLocation(FVector TargetLocation);

	/* Set LaneDestinations array. */
	void SetLaneDestinations(TArray<FVector> InLaneDestinations);

	/* Spawn given unit in free slot. */
	bool SpawnUnitInFreeSlot(TSubclassOf<class AConquestUnit> UnitToSpawn);

	/* Spawn given unit in given slot index. */
	bool SpawnUnitAtIndex(TSubclassOf<class AConquestUnit> UnitToSpawn, int32 SlotIndex);

	/* Spawn given unit in slot. */
	bool SpawnUnitInSlot(TSubclassOf<class AConquestUnit> UnitToSpawn, class AUnitSlot* UnitSlot);

private:

	bool OnSelectionGained_Implementation(AConquestPlayerController* initiator) override;
	bool OnSelectionChanged_Implementation(AConquestPlayerController* initiator, AActor* NewSelection) override;

	/* Respawn units in slots. */
	void RespawnUnitsInSlots();

	/* Respond to combat phase begin delegate. */
	UFUNCTION()
	void RespondToResourcePhaseBegin();

	/* Generate slot grid. */
	void GenerateSlotGrid();

	/* Lane destinations for spawned units. */
	TArray<FVector> LaneDestinations;

	/* Slot grid. True if occupied. */
	TMap<int32, bool> SlotGridMap;
	/* Number of unit slot grid columns. */
	static const int32 SlotGridColumnNum;
	/* Number of unit slot grid rows. */
	static const int32 SlotGridRowNum;
	/* Grid size Y. Determined by Barracks size. */
	float GridSizeY;
	/* Grid size X. Determined by Barracks size. */
	float GridSizeX;
	/* Offset to center grid Y. */
	float GridCenterOffsetY;
	/* Offset to center grid X. */
	float GridCenterOffsetX;
};
