// Fill out your copyright notice in the Description page of Project Settings.

#include "CapturePoint.h"
#include "Conquest.h"
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

FVector* ACapturePoint::GetDestinationForUnit(const FName teamName)
{
	FUnitSlot* unitSlot = GetAvailableUnitSlot(teamName);
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

void ACapturePoint::Capture(FName TeamName)
{
	OccupierTeamName = TeamName;
	UE_LOG(LogConquest, Log, TEXT("CAPTURED BY %s"), *TeamName.ToString());
}

int8 ACapturePoint::GetLaneNumber() const
{
	return LaneNumber;
}

int8 ACapturePoint::GetRowNumber() const
{
	return RowNumber;
}

void ACapturePoint::BeginPlay()
{
	Super::BeginPlay();
	GenerateUnitSlots();
}

void ACapturePoint::GenerateUnitSlots()
{
	if (HasAuthority())
	{
		AConquestGameMode* conquestGameMode = (AConquestGameMode*)GetWorld()->GetAuthGameMode();
		int32 i = 0;
		for (FName teamName : conquestGameMode->GetTeamNames())
		{
			GenerateUnitSlotsForTeam(teamName, i);
			i++;
		}
	}
}

void ACapturePoint::GenerateUnitSlotsForTeam(const FName teamName, const int32 teamIndex)
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

FUnitSlot* ACapturePoint::GetAvailableUnitSlot(const FName teamName)
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
			UE_LOG(LogConquest, Log, TEXT("Would draw"));
		}
		AreSlotsDrawn = true;
	}
}

void ACapturePoint::OnTriggerOverlapWithUnit(FName TeamName)
{
	Capture(TeamName);
}

