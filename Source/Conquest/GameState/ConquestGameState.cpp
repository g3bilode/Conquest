// Fill out your copyright notice in the Description page of Project Settings.


#include "ConquestGameState.h"
#include "../Conquest.h"
#include "../PlayerCharacter/ConquestPlayerState.h"
#include "../Units/ConquestUnit.h"
#include "Kismet/GameplayStatics.h"
#include "../GameMode/ConquestGameMode.h"


const float AConquestGameState::ResourcePhaseTime = 30.0f;

const float AConquestGameState::PhaseTransitionTime = 2.0f;


AConquestGameState::AConquestGameState()
{
}


void AConquestGameState::BeginPlay()
{
	Super::BeginPlay();

	if (HasAuthority())
	{
		AConquestGameMode* conquestGameMode = (AConquestGameMode*)GetWorld()->GetAuthGameMode();
		conquestGameMode->GameStart_OnStart.AddDynamic(this, &AConquestGameState::RespondToGameStart);
	}
}


void AConquestGameState::RespondToGameStart()
{
		// Start in resource phase
		OnResourcePhaseStart();
}


int32 AConquestGameState::CountAliveUnits() const
{
	TArray<AActor*> conquestUnitActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AConquestUnit::StaticClass(), conquestUnitActors);
	return conquestUnitActors.Num();
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


void AConquestGameState::AwardBounty(int32 Bounty, int32 KillerTeamIndex)
{
	for (AConquestPlayerState* conquestPlayerState : GetConquestPlayerArray())
	{
		if (conquestPlayerState->TeamIndex == KillerTeamIndex)
		{
			conquestPlayerState->Gold += Bounty;
			return;
		}
	}
}


void AConquestGameState::OnResourcePhaseStart_Implementation()
{
	UE_LOG(LogConquest, Log, TEXT("Resource phase"));
	CurrentPhase = EPhase::ResourcePhase;
	
	if (HasAuthority())
	{
		// Update resource phase timer with callback
		GetWorld()->GetTimerManager().SetTimer(ResourcePhaseTimerHandle, this, &AConquestGameState::OnResourcePhaseEnd, ResourcePhaseTime, false);
		// Broadcast resource phase start
		ResourcePhase_OnStart.Broadcast();
	}
	else
	{
		GetWorld()->GetTimerManager().SetTimer(ResourcePhaseTimerHandle, ResourcePhaseTime, false);
	}
}


void AConquestGameState::OnResourcePhaseEnd_Implementation()
{
	UE_LOG(LogConquest, Log, TEXT("Resource phase end"));
	CurrentPhase = EPhase::ResourcePhaseEnd;
	
	// Broadcast resource phase start
	ResourcePhase_OnEnd.Broadcast();

	if (HasAuthority())
	{
		// Update resource phase timer with callback
		GetWorld()->GetTimerManager().SetTimer(PhaseTransitionTimerHandle, this, &AConquestGameState::OnCombatPhaseStart, PhaseTransitionTime, false);
	}
}


void AConquestGameState::OnCombatPhaseStart_Implementation()
{
	UE_LOG(LogConquest, Log, TEXT("Combat phase"));
	CurrentPhase = EPhase::CombatPhase;
	
	if (HasAuthority())
	{
		AliveUnitCount = CountAliveUnits();
		if (AliveUnitCount == 0)
		{
			// No units, no combat
			OnCombatPhaseEnd();
		}
		else
		{
			// Broadcast combat phase start
			CombatPhase_OnStart.Broadcast();
		}
	}
}


void AConquestGameState::OnCombatPhaseEnd()
{
	UE_LOG(LogConquest, Log, TEXT("Combat phase end"));
	OnResourcePhaseStart();
}


void AConquestGameState::OnUnitDeath()
{
	AliveUnitCount--;
	if (AliveUnitCount <= 0)
	{
		// All done
		OnCombatPhaseEnd();
	}
}


float AConquestGameState::GetRemainingPhaseTime() const
{
	return GetWorld()->GetTimerManager().GetTimerRemaining(ResourcePhaseTimerHandle);
}

FText AConquestGameState::GetCurrentPhaseName() const
{
	// Dumbest shit I ever seen...
	const UEnum* enumPtr = FindObject<UEnum>(ANY_PACKAGE, TEXT("EPhase"), true);
	if (IsValid(enumPtr))
	{
		return enumPtr->GetDisplayNameTextByIndex(CurrentPhase.GetValue());
	}
	return FText();
}

void AConquestGameState::SkipResourcePhase()
{
	if (HasAuthority())
	{
		GetWorld()->GetTimerManager().ClearTimer(ResourcePhaseTimerHandle);
		OnResourcePhaseEnd();
	}
}
