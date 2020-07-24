// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "ConquestGameState.generated.h"

/**
 * 
 */
UCLASS()
class CONQUEST_API AConquestGameState : public AGameStateBase
{
	GENERATED_BODY()

public:
	AConquestGameState();

	TArray<class AConquestPlayerState*> getConquestPlayerArray();
	
	// Array representation of rows per lane in map
	TArray<int8> LaneSpec;
};
