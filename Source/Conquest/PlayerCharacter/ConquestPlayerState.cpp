// Fill out your copyright notice in the Description page of Project Settings.

#include "ConquestPlayerState.h"
#include "Net/UnrealNetwork.h"


AConquestPlayerState::AConquestPlayerState()
{
	GlintWorkers = 1;
	GlintGainPerWorker = 1;
}


int32 AConquestPlayerState::GetGlintIncomePerNode()
{
	return GlintWorkers * GlintGainPerWorker;
}


void AConquestPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AConquestPlayerState, Gold);
	DOREPLIFETIME(AConquestPlayerState, Glint);
	DOREPLIFETIME(AConquestPlayerState, GlintWorkers);
	DOREPLIFETIME(AConquestPlayerState, GlintGainPerWorker);
	DOREPLIFETIME_CONDITION(AConquestPlayerState, TeamName, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(AConquestPlayerState, TeamIndex, COND_InitialOnly);
}
