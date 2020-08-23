// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "UnitSlot.generated.h"

UCLASS()
class CONQUEST_API AUnitSlot : public AActor
{
	GENERATED_BODY()
	
public:	
	/* Spawn Unit at this slot. Return true on success. */
	bool SpawnUnit(TSubclassOf<class AConquestUnit> UnitToSpawn, TArray<FVector> LaneDestinations, int32 TeamIndex, int32 LaneIndex);
	/* Occupy this slot. */
	void Occupy(class AConquestUnit* NewUnit);

	void SetIsOccupied(bool NewValue);
	bool IsOccupied() const;

private:
	class AConquestUnit* OccupyingUnit;
	bool bIsOccupied;
};
