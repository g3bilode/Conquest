// Fill out your copyright notice in the Description page of Project Settings.


#include "ConquestGameState.h"
#include "Conquest.h"
#include "PlayerCharacter/ConquestPlayerState.h"
#include "Units/ConquestUnit.h"
#include "Kismet/GameplayStatics.h"

const float AConquestGameState::ResourcePhaseTime = 10.0f;


AConquestGameState::AConquestGameState()
{
}


void AConquestGameState::BeginPlay()
{
	// Start in resource phase
	OnResourcePhaseStart();
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


void AConquestGameState::OnResourcePhaseStart()
{
	UE_LOG(LogConquest, Log, TEXT("Resource phase start"));

	CurrentPhase = EPhase::ResourcePhase;
	// Update resource phase timer
	GetWorld()->GetTimerManager().SetTimer(ResourcePhaseTimerHandle, this, &AConquestGameState::OnResourcePhaseEnd, ResourcePhaseTime, false);
}


void AConquestGameState::OnResourcePhaseEnd()
{
	UE_LOG(LogConquest, Log, TEXT("Resource phase end"));

	OnCombatPhaseStart();
}


void AConquestGameState::OnCombatPhaseStart()
{
	UE_LOG(LogConquest, Log, TEXT("Combat phase start"));

	CurrentPhase = EPhase::CombatPhase;
	// Count alive units
	TArray<AActor*> conquestUnitActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AConquestUnit::StaticClass(), conquestUnitActors);
	AliveUnitCount = conquestUnitActors.Num();
	if (AliveUnitCount == 0)
	{
		// No units, no combat
		OnCombatPhaseEnd();
	}
	else
	{
		// Wake all units
		CombatPhase_OnStart.Broadcast();
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