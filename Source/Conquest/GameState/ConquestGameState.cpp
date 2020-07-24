// Fill out your copyright notice in the Description page of Project Settings.


#include "ConquestGameState.h"
#include "PlayerCharacter/ConquestPlayerState.h"

AConquestGameState::AConquestGameState()
{
	// Setup lane/row spec. Maybe this should go somewhere else...
	LaneSpec = TArray<int8>({ 3, 1, 3 });
}

TArray<AConquestPlayerState*> AConquestGameState::getConquestPlayerArray()
{
	TArray<AConquestPlayerState*> conquestPlayerArray;
	for (APlayerState* playerState : PlayerArray)
	{
		AConquestPlayerState* conquestPlayerState = Cast<AConquestPlayerState>(playerState);
		conquestPlayerArray.Add(conquestPlayerState);
	}
	return conquestPlayerArray;
}
