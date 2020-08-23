// Fill out your copyright notice in the Description page of Project Settings.


#include "Barracks.h"
#include "../Conquest.h"
#include "UnitSlot.h"
#include "../GameState/ConquestGameState.h"

// Sets default values
ABarracks::ABarracks()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}


// Called when the game starts or when spawned
void ABarracks::BeginPlay()
{
	Super::BeginPlay();
	
	// Bind resource phase start delegate
	if (HasAuthority())
	{
		AConquestGameState* conquestGameState = (AConquestGameState*)GetWorld()->GetGameState();
		conquestGameState->ResourcePhase_OnStart.AddDynamic(this, &ABarracks::RespondToResourcePhaseBegin);
	}
}


AUnitSlot* ABarracks::GetFreeUnitSlot()
{
	for (AUnitSlot* unitSlot : UnitSlots)
	{
		if (!unitSlot->IsOccupied())
		{
			return unitSlot;
		}
	}
	return nullptr;
}


void ABarracks::SetLaneDestinations(TArray<FVector> InLaneDestinations)
{
	LaneDestinations = InLaneDestinations;
}


bool ABarracks::SpawnUnitInFreeSlot(TSubclassOf<class AConquestUnit> UnitToSpawn)
{
	AUnitSlot* freeSlot = GetFreeUnitSlot();
	if (IsValid(freeSlot))
	{
		return freeSlot->SpawnUnit(UnitToSpawn, LaneDestinations, TeamIndex, LaneIndex);
	}
	else
	{
		UE_LOG(LogConquest, Error, TEXT("No free barracks slot."));
	}
	return false;
}


void ABarracks::RespawnUnitsInSlots()
{
	for (AUnitSlot* unitSlot : UnitSlots)
	{
		if (unitSlot->IsOccupied())
		{
			bool success = unitSlot->RespawnUnit(LaneDestinations, TeamIndex, LaneIndex);
			ensure(success);
		}
	}
}


void ABarracks::RespondToResourcePhaseBegin()
{
	RespawnUnitsInSlots();
}


bool ABarracks::OnSelectionChanged_Implementation(AConquestPlayerController* initiator, AActor* NewSelection)
{
	return true;
}


bool ABarracks::OnSelectionGained_Implementation(AConquestPlayerController* initiator)
{
	UE_LOG(LogConquest, Log, TEXT("Barrack selected."));
	return true;
}
