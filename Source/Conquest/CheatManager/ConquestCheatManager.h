// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CheatManager.h"
#include "ConquestCheatManager.generated.h"

/**
 * 
 */
UCLASS()
class CONQUEST_API UConquestCheatManager : public UCheatManager
{
	GENERATED_BODY()

	/** Add gold to player at given index. If no index provided, add to all. */
	UFUNCTION(exec, Category = "Conquest Cheat Manager")
	virtual void AddGold(int32 GoldAmount, int32 PlayerIndex = -1);

	/* Kill all units. */
	UFUNCTION(exec, Category = "Conquest Cheat Manager")
	virtual void KillAll();

	/* End resource phase. */
	UFUNCTION(exec, Category = "Conquest Cheat Manager")
	virtual void SkipResourcePhase();
	
};
