// Fill out your copyright notice in the Description page of Project Settings.


#include "Barracks.h"
#include "../Conquest.h"
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

	// Set grid bounds
	FVector origin;
	FVector boxExtent;
	GetActorBounds(false, origin, boxExtent);
	GridSizeX = boxExtent.X * 2 / SlotGridRowNum;
	GridSizeY = boxExtent.Y * 2 / SlotGridColumnNum;
	GridCenterOffsetX = origin.X + boxExtent.X + (GridSizeX / 2);
	GridCenterOffsetY = origin.Y + boxExtent.Y + (GridSizeY / 2);
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


TArray<FVector> ABarracks::GetLaneDestinations() const
{
	return LaneDestinations;
}


void ABarracks::SetLaneDestinations(TArray<FVector> InLaneDestinations)
{
	LaneDestinations = InLaneDestinations;
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
	return true;
}
