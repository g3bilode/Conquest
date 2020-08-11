// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnPoint.generated.h"

UCLASS()
class CONQUEST_API AUnitSlot : public AActor
{
	GENERATED_BODY()
	
public:	

	void SetIsOccupied(bool NewValue);
	bool IsOccupied() const;

private:

	bool bIsOccupied;
};
