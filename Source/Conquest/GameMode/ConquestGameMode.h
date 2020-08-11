// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "ConquestGameMode.generated.h"


/* Combat phase begin delegate */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCombatPhaseBegin);

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

UENUM()
enum EPhase
{
	ResourcePhase,
	CombatPhase
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

	// Resources
	int32 GoldGainBase;
	int32 GoldStartingAmount;
	float UpdateResourceTimer;
	FTimerHandle UpdateResourceTimerHandle;

	// Phases
	static const float ResourcePhaseTime;
	FTimerHandle ResourcePhaseTimerHandle;
	EPhase CurrentPhase;
	int32 AliveUnitCount;


	UFUNCTION()
	void UpdateResources();

	/* On resource phase start. */
	void OnResourcePhaseStart();
	/* Callback on end of resource phase time. */
	void OnResourcePhaseEnd();

	/* On start combat phase */
	void OnCombatPhaseStart();
	/* On end combat phase */
	void OnCombatPhaseEnd();

public:
	
	AConquestGameMode();

	void PostLogin(APlayerController* NewPlayer) override;

	TArray<FName> GetTeamNames() const;


	AActor* ChoosePlayerStart_Implementation(AController* Player) override;

	/* Combat phase start delegate. */
	FCombatPhaseBegin CombatPhase_OnStart;

	/* Event on unit death. */
	void OnUnitDeath();

protected:
	void BeginPlay() override;

};

