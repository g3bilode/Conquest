// Fill out your copyright notice in the Description page of Project Settings.

#include "ConquestPlayerState.h"
#include "ConquestPlayerController.h"
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


void AConquestPlayerState::OnGlintGain_Implementation(int32 NewGlintValue)
{
	// Update glint ahead of server replication, to speed up UI update.
	Glint = NewGlintValue;
	// Display drip UI
	const AActor* playerStateOwner = GetNetOwner();
	const AConquestPlayerController* conquestPlayerController = Cast<AConquestPlayerController>(playerStateOwner);
	if (IsValid(conquestPlayerController))
	{
		conquestPlayerController->DisplayResourceDrip();
	}
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
