// Fill out your copyright notice in the Description page of Project Settings.


#include "ConquestCheatManager.h"
#include "../PlayerCharacter/ConquestPlayerState.h"
#include "../GameState/ConquestGameState.h"
#include "../Units/ConquestUnit.h"
#include "Kismet/GameplayStatics.h"


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

void UConquestCheatManager::KillAll()
{
	TArray<AActor*> conquestUnitActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AConquestUnit::StaticClass(), conquestUnitActors);
	for (AActor* conquestUnitActor : conquestUnitActors)
	{
		AConquestUnit* conquestUnit = Cast<AConquestUnit>(conquestUnitActor);
		conquestUnit->TakeDamage(conquestUnit->Health, FDamageEvent(), nullptr, nullptr);
	}
}
