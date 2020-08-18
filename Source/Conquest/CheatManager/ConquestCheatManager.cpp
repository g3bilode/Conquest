// Fill out your copyright notice in the Description page of Project Settings.


#include "ConquestCheatManager.h"
#include "PlayerCharacter/ConquestPlayerState.h"
#include "GameState/ConquestGameState.h"


DEFINE_LOG_CATEGORY_STATIC(LogConquestCheatManager, Log, All);

void UConquestCheatManager::AddGold(int32 GoldAmount, int32 PlayerIndex)
{
	AConquestGameState* conquestGameState = (AConquestGameState*) GetWorld()->GetGameState();
	TArray<AConquestPlayerState*> conquestPlayerArray = conquestGameState->GetConquestPlayerArray();
	
	int32 currentIndex = 0;
	for (AConquestPlayerState* conquestPlayerState : conquestPlayerArray)
	{
		if ((PlayerIndex == -1) || (PlayerIndex == currentIndex))
		{
			conquestPlayerState->Gold += GoldAmount;
		}
		currentIndex++;
	}
}
