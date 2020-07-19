// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ConquestGameMode.generated.h"


USTRUCT()
struct FTeamDefinition
{
	GENERATED_BODY()

	FTeamDefinition()
		: TeamName(NAME_None)
	{}
	FTeamDefinition(FName inTeamName)
		: TeamName(inTeamName)
	{}

	FName TeamName;

};


UCLASS()
class CONQUEST_API AConquestGameMode : public AGameModeBase
{
	GENERATED_BODY()

private:
	// Players
	UPROPERTY()
	TArray<class AConquestPlayerState*> CurrentPlayers;
	UPROPERTY()
	TArray<FTeamDefinition> TeamDefinitions;

	// Resources
	int32 GoldGainBase;
	int32 GoldStartingAmount;
	float UpdateResourceTimer;
	FTimerHandle UpdateResourceTimerHandle;

	UFUNCTION()
	void UpdateResources();

public:
	
	AConquestGameMode();

	void PostLogin(APlayerController* NewPlayer) override;

	const TArray<FName> getTeamNames();

protected:
	void BeginPlay() override;

};
