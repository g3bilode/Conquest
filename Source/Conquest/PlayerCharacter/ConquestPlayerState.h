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
	AConquestPlayerState() : GoldGainMultiplayer(1) {}

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Resources")
	int32 Gold;

	UPROPERTY(BlueprintReadWrite, Category = "Resources")
	int32 GoldGainMultiplayer;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Team")
	FName TeamName;

	UPROPERTY(Replicated)
	int32 TeamIndex;

};
