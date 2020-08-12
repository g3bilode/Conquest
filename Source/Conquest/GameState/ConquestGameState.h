// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ConquestGameState.generated.h"


/* Combat phase begin delegate */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCombatPhaseBegin);


UENUM()
enum EPhase
{
	ResourcePhase,
	CombatPhase
};


UCLASS()
class CONQUEST_API AConquestGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AConquestGameState();

protected:
	void BeginPlay() override;

private:

	/* On resource phase start. */
	void OnResourcePhaseStart();
	/* Callback on end of resource phase time. */
	void OnResourcePhaseEnd();

	/* On start combat phase */
	void OnCombatPhaseStart();
	/* On end combat phase */
	void OnCombatPhaseEnd();


	// Phases
	static const float ResourcePhaseTime;
	FTimerHandle ResourcePhaseTimerHandle;
	EPhase CurrentPhase;
	int32 AliveUnitCount;

public:

	TArray<class AConquestPlayerState*> GetConquestPlayerArray();

	/* Event on unit death. */
	void OnUnitDeath();

	/* Combat phase start delegate. */
	FCombatPhaseBegin CombatPhase_OnStart;

};
