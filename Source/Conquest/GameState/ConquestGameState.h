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
	NonePhase		UMETA(DisplayName = "None Phase"),
	ResourcePhase	UMETA(DisplayName = "Resource Phase"),
	CombatPhase		UMETA(DisplayName = "Combat Phase")
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
	/* Callback on end of resource phase time. */
	void OnResourcePhaseEnd();

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


	// Phases
	static const float ResourcePhaseTime;
	FTimerHandle ResourcePhaseTimerHandle;
	FTimerHandle ResourcePhaseTimerHandle_Client;
	TEnumAsByte<EPhase> CurrentPhase;
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

};
