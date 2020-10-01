// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ConquestGameMode.generated.h"


/* Game start delegate */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FGameStart);


USTRUCT()
struct FTeamDefinition
{
	GENERATED_BODY()

	FTeamDefinition()
		: TeamName(NAME_None)
		, TeamIndex(-1)
	{}
	FTeamDefinition(FName InTeamName, int32 InTeamIndex)
		: TeamName(InTeamName)
		, TeamIndex(InTeamIndex)
	{}

	FName TeamName;
	int32 TeamIndex;

};

UCLASS()
class CONQUEST_API AConquestGameMode : public AGameModeBase
{
	GENERATED_BODY()

private:
	// Players
	UPROPERTY()
	int32 ConnectedPlayersCount;
	UPROPERTY()
	TArray<class AConquestPlayerState*> CurrentPlayers;
	UPROPERTY()
	TArray<FTeamDefinition> TeamDefinitions;

	/* Resource manager*/
	UPROPERTY()
	class UResourceManager* ResourceMan;

	/* Notify start game after delay. */
	FTimerHandle GameStartTimerHandle;
	/* On game start delay */
	static const float GameStartDelayTime;


	/* Return the target number of players for a full match. */
	int32 GetTargetPlayerCount();

	/* On game start */
	void GameStart();

public:
	
	AConquestGameMode();

	void PostLogin(APlayerController* NewPlayer) override;

	TArray<FName> GetTeamNames() const;

	AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	/* Game start phase start delegate. */
	FGameStart GameStart_OnStart;


protected:
	void BeginPlay() override;

};

