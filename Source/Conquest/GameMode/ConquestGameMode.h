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
	int32 ConnectedPlayersCount;
	UPROPERTY()
	TArray<class AConquestCharacter*> CurrentCharacters;
	UPROPERTY()
	TArray<FTeamDefinition> TeamDefinitions;

	// Resources
	int32 GoldGainBase;
	int32 GoldStartingAmount;
	float UpdateResourceTimer;
	FTimerHandle UpdateResourceTimerHandle;

	UFUNCTION()
	void UpdateResources();

	class APlayerStart* FindPlayerStartForTeam(FName TeamName) const;

public:
	
	AConquestGameMode();

	void PostLogin(APlayerController* NewPlayer) override;

	TArray<FName> getTeamNames() const;

protected:
	void BeginPlay() override;

};
