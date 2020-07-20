// Fill out your copyright notice in the Description page of Project Settings.


#include "ConquestGameMode.h"
#include "Conquest.h"
#include "PlayerCharacter/ConquestPlayerState.h"
#include "PlayerCharacter/ConquestPlayerController.h"
#include "GameState/ConquestGameState.h"
#include "UserWidget.h"
#include "PlayerCharacter/ConquestCharacter.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"

AConquestGameMode::AConquestGameMode()
{
	TeamDefinitions.Add(FTeamDefinition("Aztec"));
	TeamDefinitions.Add(FTeamDefinition("Inca"));

	GoldGainBase = 5;
	GoldStartingAmount = 500;
	UpdateResourceTimer = 2.0f;

	DefaultPawnClass = nullptr;
	PlayerControllerClass = AConquestPlayerController::StaticClass();
	GameStateClass = AConquestGameState::StaticClass();
	PlayerStateClass = AConquestPlayerState::StaticClass();
	static ConstructorHelpers::FClassFinder<UUserWidget> conquestHud(TEXT("/Game/Conquest/UI/HUD/Gameplay_HUD"));
	HUDClass = conquestHud.Class;
}


void AConquestGameMode::BeginPlay()
{
	Super::BeginPlay();
	
	// Update resource timer
	GetWorld()->GetTimerManager().SetTimer(UpdateResourceTimerHandle, this, &AConquestGameMode::UpdateResources, UpdateResourceTimer, true);

	// Spawn players
	for (const FTeamDefinition teamDefinition : TeamDefinitions)
	{
		// Get player's spawn
		APlayerStart* playerStart = FindPlayerStartForTeam(teamDefinition.TeamName);
		FTransform spawnTransform = playerStart->GetActorTransform();
		// Spawn player
		AActor* spawnedCharacter = GetWorld()->SpawnActor(AConquestCharacter::StaticClass(), &spawnTransform);
		AConquestCharacter* spawnedConquestCharacter = Cast<AConquestCharacter>(spawnedCharacter);
		CurrentCharacters.Add(spawnedConquestCharacter);
	}
}


void AConquestGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	AConquestPlayerState* conquestPlayerState = Cast<AConquestPlayerState>(NewPlayer->PlayerState);
	if (conquestPlayerState != nullptr)
	{
		conquestPlayerState->TeamName = TeamDefinitions[ConnectedPlayersCount].TeamName;
		conquestPlayerState->Gold = GoldStartingAmount;
		NewPlayer->Possess(CurrentCharacters[ConnectedPlayersCount]);
		ConnectedPlayersCount++;
	}
	else
	{
		UE_LOG(LogConquest, Error, TEXT("Joined player failed cast to AConquestPlayerState"));
	}
}


TArray<FName> AConquestGameMode::getTeamNames() const
{
	TArray<FName> teamNames;
	for (FTeamDefinition teamDefinition : TeamDefinitions)
	{
		teamNames.Add(teamDefinition.TeamName);
	}
	return teamNames;
}


void AConquestGameMode::UpdateResources()
{
	for (AConquestCharacter* conquestCharacter : CurrentCharacters)
	{
		AConquestPlayerState* conquestPlayerState = (AConquestPlayerState*)conquestCharacter->GetPlayerState();
		conquestPlayerState->Gold += conquestPlayerState->GoldGainMultiplayer * GoldGainBase;
	}
}


APlayerStart* AConquestGameMode::FindPlayerStartForTeam(FName TeamName) const
{
	for (TActorIterator<APlayerStart> PlayerStartItr(GetWorld()); PlayerStartItr; ++PlayerStartItr)
	{
		if (TeamName == PlayerStartItr->PlayerStartTag)
		{
			return *PlayerStartItr;
		}
	}
	UE_LOG(LogConquest, Error, TEXT("FAILED TO FIND PLAYER START"));
	return nullptr;
}
