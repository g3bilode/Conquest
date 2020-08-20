// Fill out your copyright notice in the Description page of Project Settings.

#include "Capital.h"
#include "../Conquest.h"
#include "Kismet/GameplayStatics.h"
#include "../PlayerCharacter/ConquestPlayerController.h"
#include "../PlayerCharacter/ConquestPlayerState.h"

// Sets default values
ACapital::ACapital()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MaxHealth = 100.f;
	CurrentHealth = MaxHealth;
}

bool ACapital::OnSelectionGained_Implementation(AConquestPlayerController* initiator)
{
	AConquestPlayerState* conquestPlayerState = initiator->GetConquestPlayerState();
	if (IsValid(conquestPlayerState))
	{
		if (conquestPlayerState->TeamName == TeamName)
		{
			// Friendly
			initiator->DisplayCapitalMenu(GetActorLocation());
		}
	}
	return true;
}

bool ACapital::OnSelectionChanged_Implementation(AConquestPlayerController* initiator, AActor* NewSelection)
{
	initiator->SetCapitalMenuVisibility(false);
	return true;
}
