// Fill out your copyright notice in the Description page of Project Settings.

#include "CapturePoint.h"
#include "../Conquest.h"
#include "../GameState/ConquestGameState.h"
#include "../PlayerCharacter/ConquestPlayerState.h"
#include "../GameMode/ConquestGameMode.h"
#include "Net/UnrealNetwork.h"


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
	OccupierTeamIndex = -1;
}


void ACapturePoint::Capture(int32 TeamIndex)
{
	OccupierTeamIndex = TeamIndex;
	DisplayCapture();
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
}


void ACapturePoint::OnTriggerOverlapWithUnit(int32 TeamIndex)
{
	if (TeamIndex != OccupierTeamIndex)
	{
		Capture(TeamIndex);
	}
}

