// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "ResourceManager.generated.h"


UCLASS()
class CONQUEST_API UResourceManager : public UObject
{
	GENERATED_BODY()

public:
	UResourceManager();

private:
	/* Base gold gain amount for all players. */
	static const int32 GoldGainBase;
	/* Base gold starting amount for all players. */
	static const int32 GoldStartingAmount;
	/* Time between resource update ticks. */
	static const float UpdateResourceTime;
	/* Resource timer handle. */
	FTimerHandle UpdateResourceTimerHandle;

	/* List of in-game player states. */
	TArray<class AConquestPlayerState*> ConquestPlayerStates;
	/* List of CapturePoints. */
	//UPROPERTY()
	TArray<class ACapturePoint*> CapturePoints;
	/* Active world reference. */
	UWorld* ActiveWorld;


	/* Update resource amounts for each player. */
	UFUNCTION()
	void UpdateResources();
	/* Return the amount of glint gain per drip for given player. */
	int32 AccumulateGlintIncome(class AConquestPlayerState* ConquestPlayerState);


	/* Event on resource phase begin. */
	UFUNCTION()
	void RespondToResourcePhaseBegin();
	/* Event on resource phase end. */
	UFUNCTION()
	void RespondToResourcePhaseEnd();

public:
	/* Setup ResourceManager with given players. */
	void Setup(TArray<class AConquestPlayerState*> InConquestPlayerStates, UWorld* InWorld);

};
