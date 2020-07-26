// Fill out your copyright notice in the Description page of Project Settings.


#include "ConquestGameState.h"
#include "PlayerCharacter/ConquestPlayerState.h"

AConquestGameState::AConquestGameState()
{
}

TArray<class AConquestPlayerState*> AConquestGameState::GetConquestPlayerArray()
{
	TArray<AConquestPlayerState*> conquestPlayerArray;
	for (APlayerState* playerState : PlayerArray)
	{
		AConquestPlayerState* conquestPlayerState = Cast<AConquestPlayerState>(playerState);
		conquestPlayerArray.Add(conquestPlayerState);
	}
	return conquestPlayerArray;
}
