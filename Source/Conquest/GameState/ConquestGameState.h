// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ConquestGameState.generated.h"


/* Combat phase begin delegate */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FCombatPhaseBegin);
/* Resource phase begin delegate */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FResourcePhaseBegin);
/* Resource phase end delegate */
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FResourcePhaseEnd);


UENUM()
enum EPhase
{
	NonePhase			UMETA(DisplayName = "None Phase"),
	ResourcePhase		UMETA(DisplayName = "Resource Phase"),
	ResourcePhaseEnd	UMETA(DisplayName = "Resource Phase End"),
	CombatPhase			UMETA(DisplayName = "Combat Phase")
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

	/* Multicast to clients on resource phase start. */
	UFUNCTION(NetMulticast, Reliable)
	void OnResourcePhaseStart();
	virtual void OnResourcePhaseStart_Implementation();
	/* Multicast to clients on resource phase end. */
	UFUNCTION(NetMulticast, Reliable)
	void OnResourcePhaseEnd();
	virtual void OnResourcePhaseEnd_Implementation();


	/* Multicast to clients on combat phase start. */
	UFUNCTION(NetMulticast, Reliable)
	void OnCombatPhaseStart();
	virtual void OnCombatPhaseStart_Implementation();
	/* On end combat phase */
	void OnCombatPhaseEnd();

	UFUNCTION()
	/* Respond to Game start delegate */
	void RespondToGameStart();

	/* Count alive units. */
	int32 CountAliveUnits() const;


	/* Duration of the resource phase. */
	static const float ResourcePhaseTime;
	/* Timer for resource phase. */
	FTimerHandle ResourcePhaseTimerHandle;
	/* Duration of the phase transition. */
	static const float PhaseTransitionTime;
	/* Timer for phase transition. */
	FTimerHandle PhaseTransitionTimerHandle;
	/* Current phase. */
	TEnumAsByte<EPhase> CurrentPhase;
	/* Count of alive units for current combat phase. */
	int32 AliveUnitCount;

public:

	TArray<class AConquestPlayerState*> GetConquestPlayerArray();

	/* Event on unit death. */
	void OnUnitDeath();

	UFUNCTION(BlueprintCallable)
	float GetRemainingPhaseTime() const;

	UFUNCTION(BlueprintCallable)
	FText GetCurrentPhaseName() const;

	/* Skip resource phase. */
	void SkipResourcePhase();

	/* Combat phase start delegate. */
	FCombatPhaseBegin CombatPhase_OnStart;
	/* Resource phase start delegate. */
	FResourcePhaseBegin ResourcePhase_OnStart;
	/* Resource phase end delegate. */
	FResourcePhaseEnd ResourcePhase_OnEnd;
};
