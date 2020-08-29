// Fill out your copyright notice in the Description page of Project Settings.

#include "UnitSlot.h"
#include "../Units/ConquestUnit.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"



bool AUnitSlot::SpawnUnit(TSubclassOf<class AConquestUnit> UnitToSpawn, TArray<FVector> LaneDestinations, int32 TeamIndex, int32 LaneIndex)
{
	FVector spawnLocation = GetActorLocation() + FVector(0, 0, 200);
	FTransform spawnTransform;
	spawnTransform.SetLocation(spawnLocation);

	AActor* spawnedUnit = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), UnitToSpawn->GetDefaultObject()->GetClass(), spawnTransform, ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding, this);
	AConquestUnit* conquestSpawnedUnit = Cast<AConquestUnit>(spawnedUnit);

	if (IsValid(conquestSpawnedUnit))
	{
		conquestSpawnedUnit->TeamIndex = TeamIndex;
		conquestSpawnedUnit->LaneIndex = LaneIndex;
		conquestSpawnedUnit->UnitSlot = this;
		conquestSpawnedUnit->SetLaneDestinations(LaneDestinations);

		UGameplayStatics::FinishSpawningActor(conquestSpawnedUnit, spawnTransform);
		return true;
	}
	return false;
}


bool AUnitSlot::RespawnUnit(TArray<FVector> LaneDestinations, int32 TeamIndex, int32 LaneIndex)
{
	return SpawnUnit(OccupyingClass, LaneDestinations, TeamIndex, LaneIndex);
}


void AUnitSlot::Occupy(AConquestUnit* NewUnit)
{
	OccupyingUnit = NewUnit;
	OccupyingClass = NewUnit->GetClass();
	SetIsOccupied(true);
	UnitSlot_OnUpdate.ExecuteIfBound();
}


void AUnitSlot::SetIsOccupied(bool NewValue)
{
	bIsOccupied = NewValue;
}


bool AUnitSlot::IsOccupied() const
{
	return bIsOccupied;
}

AConquestUnit* AUnitSlot::GetOccupyingUnit() const
{
	return OccupyingUnit;
}
