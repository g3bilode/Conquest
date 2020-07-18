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
	TArray<class AConquestPlayerState*> getConquestPlayerArray();
	
};
