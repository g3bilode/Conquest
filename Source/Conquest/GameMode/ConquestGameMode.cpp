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
	// Team Definitions
	TeamDefinitions.Add(FTeamDefinition("Aztec", 0));
	TeamDefinitions.Add(FTeamDefinition("Inca", 1));

	// Game constants
	GoldGainBase = 5;
	GoldStartingAmount = 500;
	UpdateResourceTimer = 2.0f;

	// Default Classes
	DefaultPawnClass = AConquestCharacter::StaticClass();
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
		conquestPlayerState->Gold = GoldStartingAmount;
		ConnectedPlayersCount++;
	}
	else
	{
		UE_LOG(LogConquest, Error, TEXT("Joined player failed cast to AConquestPlayerState"));
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


void AConquestGameMode::UpdateResources()
{
	for (AConquestPlayerState* conquestPlayerState : CurrentPlayers)
	{
		conquestPlayerState->Gold += conquestPlayerState->GoldGainMultiplayer * GoldGainBase;
	}
}
