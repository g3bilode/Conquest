// Fill out your copyright notice in the Description page of Project Settings.


#include "ConquestGameMode.h"
#include "../Conquest.h"
#include "../GameState/ConquestGameState.h"
#include "../HUD/ConquestHUD.h"
#include "../PlayerCharacter/ConquestCharacter.h"
#include "../PlayerCharacter/ConquestPlayerState.h"
#include "../PlayerCharacter/ConquestPlayerController.h"
#include "GameFramework/PlayerStart.h"
#include "EngineUtils.h"
#include "UMG/Public/Blueprint/UserWidget.h"
#include "../Resources/ResourceManager.h"


const float AConquestGameMode::GameStartDelayTime = 1.0f;


AConquestGameMode::AConquestGameMode()
{
	// Team Definitions
	TeamDefinitions.Add(FTeamDefinition("Aztec", 0));
	TeamDefinitions.Add(FTeamDefinition("Inca", 1));

	// Default Classes
	DefaultPawnClass = AConquestCharacter::StaticClass();
	PlayerControllerClass = AConquestPlayerController::StaticClass();
	GameStateClass = AConquestGameState::StaticClass();
	PlayerStateClass = AConquestPlayerState::StaticClass();
	HUDClass = AConquestHUD::StaticClass();

	ResourceMan = NewObject<UResourceManager>();
}


void AConquestGameMode::BeginPlay()
{
	Super::BeginPlay();
}


void AConquestGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	AConquestPlayerState* conquestPlayerState = Cast<AConquestPlayerState>(NewPlayer->PlayerState);
	if (conquestPlayerState != nullptr)
	{
		CurrentPlayers.Add(conquestPlayerState);
		conquestPlayerState->TeamName = TeamDefinitions[ConnectedPlayersCount].TeamName;
		conquestPlayerState->TeamIndex = TeamDefinitions[ConnectedPlayersCount].TeamIndex;
		ConnectedPlayersCount++;
	}
	else
	{
		UE_LOG(LogConquest, Error, TEXT("Joined player failed cast to AConquestPlayerState"));
	}
	if (ConnectedPlayersCount == GetTargetPlayerCount())
	{
		// Notify game start after delay
		// TODO: Improve this with RPC from client?
		GetWorld()->GetTimerManager().SetTimer(GameStartTimerHandle, this, &AConquestGameMode::GameStart, GameStartDelayTime, false);
	}
}


TArray<FName> AConquestGameMode::GetTeamNames() const
{
	TArray<FName> teamNames;
	for (FTeamDefinition teamDefinition : TeamDefinitions)
	{
		teamNames.Add(teamDefinition.TeamName);
	}
	return teamNames;
}


AActor* AConquestGameMode::ChoosePlayerStart_Implementation(AController* Player)
{
	FName TeamName = GetTeamNames()[ConnectedPlayersCount];
	// ^ Only works if PostLogin for this player runs before the next player chooses a start. Is this guaranteed?
	// If not, just setup another var...
	for (TActorIterator<APlayerStart> PlayerStartItr(GetWorld()); PlayerStartItr; ++PlayerStartItr)
	{
		if (TeamName == PlayerStartItr->PlayerStartTag)
		{
			return *PlayerStartItr;
		}
	}
	UE_LOG(LogConquest, Error, TEXT("Failed to find player start for %s"), *GetNameSafe(Player));
	return nullptr;
}


int32 AConquestGameMode::GetTargetPlayerCount()
{
	return TeamDefinitions.Num();
}


void AConquestGameMode::GameStart()
{
	// Setup resource manager
	ResourceMan->Setup(CurrentPlayers, GetWorld());

	// Notify everyone of game start
	GameStart_OnStart.Broadcast();
}
