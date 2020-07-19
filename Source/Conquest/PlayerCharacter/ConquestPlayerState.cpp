// Fill out your copyright notice in the Description page of Project Settings.

#include "ConquestPlayerState.h"
#include "UnrealNetwork.h"

void AConquestPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AConquestPlayerState, Gold);
	DOREPLIFETIME(AConquestPlayerState, TeamName);
}
