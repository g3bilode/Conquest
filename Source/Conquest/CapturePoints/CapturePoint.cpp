// Fill out your copyright notice in the Description page of Project Settings.

#include "CapturePoint.h"
#include "Conquest.h"
#include "DrawDebugHelpers.h"


bool ACapturePoint::OnSelectionGained_Implementation(AConquestPlayerController* initiator)
{
	UE_LOG(LogConquest, Log, TEXT("I am Selected! (%s)"), *GetNameSafe(this));
	return true;
}

bool ACapturePoint::OnSelectionChanged_Implementation(AConquestPlayerController* initiator, AActor* NewSelection)
{
	return true;
}

ACapturePoint::ACapturePoint()
{
	unitSlotCount = 6;
	unitSlotColumnCount = 3;
	unitSlotBuffer = 100;
}

FVector* ACapturePoint::getDestinationForUnit()
{
	FUnitSlot* unitSlot = getAvailableUnitSlot();
	if (unitSlot != nullptr)
	{
		// Found a free guy
		unitSlot->isOccupied = true;
		return &unitSlot->slotLocation;
	}
	// No free guy
	UE_LOG(LogConquest, Log, TEXT("No unit slot available."));
	return nullptr;
}

void ACapturePoint::BeginPlay()
{
	Super::BeginPlay();
	generateUnitSlots();
}

void ACapturePoint::generateUnitSlots()
{
	int32 row;
	int32 column;
	FVector slotLocation;
	for (int32 i=0; i<unitSlotCount; i++)
	{
		row = floor(i / unitSlotColumnCount) - (unitSlotCount / unitSlotColumnCount / 2);
		column = (i % unitSlotColumnCount) - (unitSlotColumnCount / 2);
		slotLocation = FVector(row*unitSlotBuffer, column*unitSlotBuffer, 0);
		slotLocation += GetActorLocation();
		FUnitSlot unitSlot = FUnitSlot(slotLocation);
		unitSlots.Add(unitSlot);
		DrawDebugSphere(GetWorld(), slotLocation, 10.0f, 32, FColor(100, 0, 0), true);
	}
}

FUnitSlot* ACapturePoint::getAvailableUnitSlot()
{
	for (FUnitSlot& unitSlot : unitSlots)
	{
		if (!unitSlot.isOccupied)
		{
			return &unitSlot;
		}
	}
	return nullptr;
}
