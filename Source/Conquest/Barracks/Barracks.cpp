// Fill out your copyright notice in the Description page of Project Settings.


#include "Barracks.h"
#include "../Conquest.h"
#include "UnitSlot.h"
#include "../GameState/ConquestGameState.h"
#include "../HUD/ConquestHUD.h"
#include "../PlayerCharacter/ConquestPlayerController.h"


const int32 ABarracks::SlotGridColumnNum(4);

const int32 ABarracks::SlotGridRowNum(3);


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

	// Set grid bounds
	FVector origin;
	FVector boxExtent;
	GetActorBounds(false, origin, boxExtent);
	GridSizeX = boxExtent.X * 2 / SlotGridRowNum;
	GridSizeY = boxExtent.Y * 2 / SlotGridColumnNum;
	GridCenterOffsetX = origin.X + boxExtent.X + (GridSizeX / 2);
	GridCenterOffsetY = origin.Y + boxExtent.Y + (GridSizeY / 2);


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


class AUnitSlot* ABarracks::GetUnitSlotAtIndex(int32 SlotIndex)
{
	if (ensure(UnitSlots.IsValidIndex(SlotIndex)))
	{
		return UnitSlots[SlotIndex];
	}
	return nullptr;
}


FVector ABarracks::GetNearestFreeSlotLocation(FVector TargetLocation)
{
	int32 rowNumber = GetSlotRow(TargetLocation.X);
	int32 columnNumber = GetSlotColumn(TargetLocation.Y);
	int32 slotID = GetSlotIDFromRowColumn(rowNumber, columnNumber);
	if (!IsSlotOccupied(slotID))
	{
		// Valid slot, let's snap
		float newX = (rowNumber * GridSizeX) + GridCenterOffsetX;
		float newY = (columnNumber * GridSizeY) + GridCenterOffsetY;
		return FVector(newX, newY, TargetLocation.Z);
	}
	else
	{
		// Occupied, don't snap
		return TargetLocation;
	}
}


void ABarracks::SetLaneDestinations(TArray<FVector> InLaneDestinations)
{
	LaneDestinations = InLaneDestinations;
}


bool ABarracks::SpawnUnitInFreeSlot(TSubclassOf<class AConquestUnit> UnitToSpawn)
{
	// TODO Remove this function
	AUnitSlot* freeSlot = GetFreeUnitSlot();
	if (IsValid(freeSlot))
	{
		return SpawnUnitInSlot(UnitToSpawn, freeSlot);
	}
	else
	{
		UE_LOG(LogConquest, Error, TEXT("No free barracks slot."));
		return false;
	}
}


bool ABarracks::SpawnUnitAtIndex(TSubclassOf<class AConquestUnit> UnitToSpawn, int32 SlotIndex)
{
	AUnitSlot* indexedSlot = GetUnitSlotAtIndex(SlotIndex);
	if (IsValid(indexedSlot))
	{
		return SpawnUnitInSlot(UnitToSpawn, indexedSlot);
	}
	else
	{
		UE_LOG(LogConquest, Error, TEXT("Invalid slot index."));
		return false;
	}
}


bool ABarracks::SpawnUnitInSlot(TSubclassOf<class AConquestUnit> UnitToSpawn, class AUnitSlot* UnitSlot)
{
	if (IsValid(UnitSlot))
	{
		if (UnitSlot->IsOccupied())
		{
			UE_LOG(LogConquest, Error, TEXT("Slot already occupied."));
			return false;
		}
		return UnitSlot->SpawnUnit(UnitToSpawn, LaneDestinations, TeamIndex, LaneIndex);
	}
	else
	{
		UE_LOG(LogConquest, Error, TEXT("Invalid slot provided."));
		return false;
	}
}


bool ABarracks::OccupySlot(int32 SlotID)
{
	if (!IsSlotOccupied(SlotID))
	{
		SlotGridMap.Add(SlotID, true);
		return true;
	}
	return false;
}


bool ABarracks::IsSlotOccupied(int32 SlotID) const
{
	bool slotValue = SlotGridMap.Find(SlotID);
	return slotValue;
}


int32 ABarracks::GetSlotIDFromLocation(FVector Location)
{
	int32 columnNumber = GetSlotColumn(Location.Y);
	int32 rowNumber = GetSlotRow(Location.X);
	return GetSlotIDFromRowColumn(rowNumber, columnNumber);
}


int32 ABarracks::GetSlotIDFromRowColumn(int32 Row, int32 Column)
{
	return Row * 1000 + Column;
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


int32 ABarracks::GetSlotColumn(float Y) const
{
	return GetGridAxisValue(Y, GridCenterOffsetY, GridSizeY);
}


int32 ABarracks::GetSlotRow(float X) const
{
	return GetGridAxisValue(X, GridCenterOffsetX, GridSizeX);
}


int32 ABarracks::GetGridAxisValue(float AxisValue, float AxisOffset, float AxisGridSize) const
{
	return round((AxisValue - AxisOffset) / AxisGridSize);
}


bool ABarracks::OnSelectionChanged_Implementation(AConquestPlayerController* initiator, AActor* NewSelection)
{
	return true;
}


bool ABarracks::OnSelectionGained_Implementation(AConquestPlayerController* initiator)
{
	if (initiator->GetConquestPlayerState()->TeamIndex == TeamIndex)
	{
		// Friendly
		AConquestHUD* conquestHUD = (AConquestHUD*) initiator->GetHUD();
		conquestHUD->OnBarrackSelection(UnitSlots, LaneIndex);
	}
	return true;
}
