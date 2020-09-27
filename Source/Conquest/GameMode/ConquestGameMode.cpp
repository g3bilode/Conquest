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
#include "../CapturePoints/CapturePoint.h"


const float AConquestGameMode::GameStartDelayTime = 0.5f;


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
	HUDClass = AConquestHUD::StaticClass();
}


void AConquestGameMode::BeginPlay()
{
	Super::BeginPlay();

	AConquestGameState* conquestGameState = GetGameState<AConquestGameState>();
	if (IsValid(conquestGameState))
	{
		conquestGameState->ResourcePhase_OnStart.AddDynamic(this, &AConquestGameMode::RespondToResourcePhaseBegin);
	}

	// Gather CapturePoints
	for (TActorIterator<ACapturePoint> CapturePointItr(GetWorld()); CapturePointItr; ++CapturePointItr)
	{
		CapturePoints.Add(*CapturePointItr);
	}
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


void AConquestGameMode::UpdateResources()
{
	for (AConquestPlayerState* conquestPlayerState : CurrentPlayers)
	{
		conquestPlayerState->Gold += GoldGainBase;
		conquestPlayerState->Glint += AccumulateGlintIncome(conquestPlayerState);
		// Display +gain in world
		const AActor* playerStateOwner = conquestPlayerState->GetNetOwner();
		const AConquestPlayerController* conquestPlayerController = Cast<AConquestPlayerController>(playerStateOwner);
		if (IsValid(conquestPlayerController))
		{
			conquestPlayerController->DisplayResourceDrip();
		}
	}
}


int32 AConquestGameMode::AccumulateGlintIncome(AConquestPlayerState* ConquestPlayerState)
{
	int32 glintPerNode = ConquestPlayerState->GetGlintIncomePerNode();
	int32 totalIncome = 0;
	for (ACapturePoint* CapturePoint : CapturePoints)
	{
		if (CapturePoint->OccupierTeamIndex == ConquestPlayerState->TeamIndex)
		{
			// Friendly
			totalIncome += glintPerNode;
		}
	}
	return totalIncome;
}


int32 AConquestGameMode::GetTargetPlayerCount()
{
	return TeamDefinitions.Num();
}


void AConquestGameMode::GameStart()
{
	// Update resource timer
	GetWorld()->GetTimerManager().SetTimer(UpdateResourceTimerHandle, this, &AConquestGameMode::UpdateResources, UpdateResourceTimer, true);

	// Notify everyone of game start
	GameStart_OnStart.Broadcast();
}


void AConquestGameMode::RespondToResourcePhaseBegin()
{
	// Get glint bonus per team
	CapturePointsPerTeam.Empty();
	for (TActorIterator<ACapturePoint> CapturePointItr(GetWorld()); CapturePointItr; ++CapturePointItr)
	{
		int32 occupierIndex = CapturePointItr->OccupierTeamIndex;
		int32& currentValue = CapturePointsPerTeam.FindOrAdd(occupierIndex);
		CapturePointsPerTeam[occupierIndex] = currentValue + 1;		
	}
}
