// Fill out your copyright notice in the Description page of Project Settings.


#include "ConquestGameMode.h"
#include "Conquest.h"
#include "PlayerCharacter/ConquestPlayerState.h"
#include "PlayerCharacter/ConquestPlayerController.h"
#include "GameState/ConquestGameState.h"
#include "UserWidget.h"
#include "PlayerCharacter/ConquestCharacter.h"

AConquestGameMode::AConquestGameMode()
{
	TeamDefinitions.Add(FTeamDefinition("Aztec"));
	TeamDefinitions.Add(FTeamDefinition("Inca"));

	GoldGainBase = 5;
	GoldStartingAmount = 500;
	UpdateResourceTimer = 2.0f;

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
		const int32 idx = CurrentPlayers.Num() - 1;
		conquestPlayerState->TeamName = TeamDefinitions[idx].TeamName;
		conquestPlayerState->Gold = GoldStartingAmount;
	}
	else
	{
		UE_LOG(LogConquest, Error, TEXT("Joined player failed cast to AConquestPlayerState"));
	}
}


void AConquestGameMode::UpdateResources()
{
	for (AConquestPlayerState* conquestPlayerState : CurrentPlayers)
	{
		conquestPlayerState->Gold += conquestPlayerState->GoldGainMultiplayer * GoldGainBase;
	}
}
