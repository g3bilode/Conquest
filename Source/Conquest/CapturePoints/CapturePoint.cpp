// Fill out your copyright notice in the Description page of Project Settings.

#include "CapturePoint.h"
#include "Conquest.h"
#include "DrawDebugHelpers.h"
#include "GameState/ConquestGameState.h"
#include "PlayerCharacter/ConquestPlayerState.h"
#include "GameMode/ConquestGameMode.h"


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
	UnitSlotCount = 6;
	UnitSlotColumnCount = 3;
	UnitSlotBuffer = 100;
}

FVector* ACapturePoint::getDestinationForUnit(const FName teamName)
{
	FUnitSlot* unitSlot = getAvailableUnitSlot(teamName);
	if (unitSlot != nullptr)
	{
		// Found a free guy
		unitSlot->IsOccupied = true;
		return &unitSlot->SlotLocation;
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
	if (HasAuthority())
	{
		AConquestGameMode* conquestGameMode = (AConquestGameMode*)GetWorld()->GetAuthGameMode();
		

	}


	AConquestGameState* conquestGameState = (AConquestGameState*)GetWorld()->GetGameState();
	int32 i = 0;
	for (AConquestPlayerState* playerState : conquestGameState->getConquestPlayerArray())
	{
		generateUnitSlotsForTeam(playerState->TeamName, i);
		i++;
	}
}

void ACapturePoint::generateUnitSlotsForTeam(const FName teamName, const int32 teamIndex)
{
	for (int32 i=0; i<UnitSlotCount; i++)
	{
		// TODO improve this
		int32 y_offset = 200 * teamIndex - 100;
		int32 row = floor(i / UnitSlotColumnCount) - (UnitSlotCount / UnitSlotColumnCount / 2);
		int32 column = (i % UnitSlotColumnCount) - (UnitSlotColumnCount / 2);
		FVector slotLocation = FVector(row*UnitSlotBuffer, column*UnitSlotBuffer+y_offset, 0);
		slotLocation += GetActorLocation();
		FUnitSlot unitSlot = FUnitSlot(slotLocation, teamName);
		UnitSlots.Add(unitSlot);
		DrawDebugSphere(GetWorld(), slotLocation, 10.0f, 32, FColor(100, 0, 0), true);
	}
}

FUnitSlot* ACapturePoint::getAvailableUnitSlot(const FName teamName)
{
	for (FUnitSlot& unitSlot : UnitSlots)
	{
		if (!unitSlot.IsOccupied && unitSlot.TeamName == teamName)
		{
			return &unitSlot;
		}
	}
	return nullptr;
}

