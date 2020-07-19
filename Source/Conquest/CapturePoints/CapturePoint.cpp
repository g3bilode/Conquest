// Fill out your copyright notice in the Description page of Project Settings.

#include "CapturePoint.h"
#include "Conquest.h"
#include "DrawDebugHelpers.h"
#include "GameState/ConquestGameState.h"
#include "PlayerCharacter/ConquestPlayerState.h"
#include "GameMode/ConquestGameMode.h"
#include "UnrealNetwork.h"


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
	UnitSlotCount = 4;
	UnitSlotColumnCount = 2;
	UnitSlotBuffer = 100;
	AreSlotsDrawn = false;
}

void ACapturePoint::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ACapturePoint, UnitSlots);
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
		int32 i = 0;
		for (FName teamName : conquestGameMode->getTeamNames())
		{
			generateUnitSlotsForTeam(teamName, i);
			i++;
		}
	}
}

void ACapturePoint::generateUnitSlotsForTeam(const FName teamName, const int32 teamIndex)
{
	for (int32 i=0; i<UnitSlotCount; i++)
	{
		// TODO: improve this?
		int32 x_offset = 400 * teamIndex - 200;
		int32 row = floor(i / UnitSlotColumnCount) - (UnitSlotCount / UnitSlotColumnCount / 2);
		int32 column = (i % UnitSlotColumnCount) - (UnitSlotColumnCount / 2);
		FVector slotLocation = FVector(row*UnitSlotBuffer+x_offset, column*UnitSlotBuffer, 0);
		slotLocation += GetActorLocation();
		FUnitSlot unitSlot = FUnitSlot(slotLocation, teamName);
		UnitSlots.Add(unitSlot);
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

void ACapturePoint::OnRep_UnitSlots()
{
	if (!AreSlotsDrawn)
	{
		for (FUnitSlot unitSlot : UnitSlots)
		{
			DrawDebugSphere(GetWorld(), unitSlot.SlotLocation, 10.0f, 32, FColor(100, 0, 0), true);
		}
		AreSlotsDrawn = true;
	}
}

