// Fill out your copyright notice in the Description page of Project Settings.

#include "Outpost.h"
#include "../Conquest.h"
#include "Kismet/GameplayStatics.h"
#include "../PlayerCharacter/ConquestPlayerController.h"
#include "../PlayerCharacter/ConquestPlayerState.h"

// Sets default values
AOutpost::AOutpost()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MaxHealth = 100.f;
	CurrentHealth = MaxHealth;
}

bool AOutpost::OnSelectionGained_Implementation(AConquestPlayerController* initiator)
{
	UE_LOG(LogConquest, Log, TEXT("Outpost Selected! (%s)"), *GetNameSafe(this));
	initiator->OnOutpostSelect(this);
	AConquestPlayerState* conquestPlayerState = initiator->GetConquestPlayerState();
	if (IsValid(conquestPlayerState))
	{
		if (conquestPlayerState->TeamName == TeamName)
		{
			// Friendly
			initiator->DisplayOutpostMenu(GetActorLocation());
		}
		else
		{
			// Enemy
			initiator->AttackOutpost(this);
		}
	}
	return true;
}

bool AOutpost::OnSelectionChanged_Implementation(AConquestPlayerController* initiator, AActor* NewSelection)
{
	initiator->SetOutpostMenuVisibility(false);
	return true;
}
