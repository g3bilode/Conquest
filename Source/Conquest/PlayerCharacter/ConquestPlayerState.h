// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerState.h"
#include "ConquestPlayerState.generated.h"

/**
 * 
 */
UCLASS()
class CONQUEST_API AConquestPlayerState : public APlayerState
{
	GENERATED_BODY()

public:
	AConquestPlayerState();

	/* Gold resource. */
	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Resources")
	int32 Gold;

	/* Glint resource. */
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Resources")
	int32 Glint;

	/* Glint gained per worker. */
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Resources")
	int32 GlintGainPerWorker;

	/* Number of Glint workers. */
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Resources")
	int32 GlintWorkers;

	/* Player's team name. */
	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Team")
	FName TeamName;

	/* Player's, team index. */
	UPROPERTY(Replicated)
	int32 TeamIndex;

	/* Get glint income per node. */
	int32 GetGlintIncomePerNode();

	/* Function on glint gain, RPC to client. */
	UFUNCTION(Client, Unreliable)
	void OnGlintGain(int32 NewGlintValue);
	virtual void OnGlintGain_Implementation(int32 NewGlintValue);

};
