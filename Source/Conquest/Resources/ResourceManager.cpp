// Fill out your copyright notice in the Description page of Project Settings.


#include "ResourceManager.h"
#include "../CapturePoints/CapturePoint.h"
#include "../PlayerCharacter/ConquestPlayerState.h"
#include "EngineUtils.h"
#include "Engine/World.h"
#include "../GameState/ConquestGameState.h"


const int32 UResourceManager::GoldGainBase(5);

const int32 UResourceManager::GoldStartingAmount(500);

const float UResourceManager::UpdateResourceTime(3.0f);


UResourceManager::UResourceManager()
{
}


void UResourceManager::Setup(TArray<class AConquestPlayerState*> InConquestPlayerStates, UWorld* InWorld)
{
	// Set players
	ConquestPlayerStates = InConquestPlayerStates;
	// Set world
	ActiveWorld = InWorld;
	// Gather CapturePoints
	for (TActorIterator<ACapturePoint> CapturePointItr(ActiveWorld); CapturePointItr; ++CapturePointItr)
	{
		CapturePoints.Add(*CapturePointItr);
	}
	// Bind to resource phase status
	AConquestGameState* conquestGameState = ActiveWorld->GetGameState<AConquestGameState>();
	if (IsValid(conquestGameState))
	{
		conquestGameState->ResourcePhase_OnStart.AddDynamic(this, &UResourceManager::RespondToResourcePhaseBegin);
		conquestGameState->ResourcePhase_OnEnd.AddDynamic(this, &UResourceManager::RespondToResourcePhaseEnd);
	}
	// Give each player starting gold.
	for (AConquestPlayerState* conquestPlayerState : ConquestPlayerStates)
	{
		conquestPlayerState->Gold = GoldStartingAmount;
	}
	// Start resource timer
	ActiveWorld->GetTimerManager().SetTimer(UpdateResourceTimerHandle, this, &UResourceManager::UpdateResources, UpdateResourceTime, true);
}


void UResourceManager::UpdateResources()
{
	for (AConquestPlayerState* conquestPlayerState : ConquestPlayerStates)
	{
		conquestPlayerState->Gold += GoldGainBase;
		conquestPlayerState->Glint += AccumulateGlintIncome(conquestPlayerState);
		conquestPlayerState->OnGlintGain(conquestPlayerState->Glint);
	}
}


int32 UResourceManager::AccumulateGlintIncome(AConquestPlayerState* ConquestPlayerState)
{
	int32 glintPerNode = ConquestPlayerState->GetGlintIncomePerNode();
	int32 totalIncome = 0;
	for (ACapturePoint* CapturePoint : CapturePoints)
	{
		if (CapturePoint->OccupierTeamIndex == ConquestPlayerState->TeamIndex)
		{
			// Friendly
			totalIncome += glintPerNode;
		}
	}
	return totalIncome;
}


void UResourceManager::RespondToResourcePhaseBegin()
{
	ActiveWorld->GetTimerManager().UnPauseTimer(UpdateResourceTimerHandle);
}


void UResourceManager::RespondToResourcePhaseEnd()
{
	ActiveWorld->GetTimerManager().PauseTimer(UpdateResourceTimerHandle);
}