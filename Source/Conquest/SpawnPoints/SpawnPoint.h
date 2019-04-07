// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "SpawnPoint.generated.h"

UCLASS()
class CONQUEST_API ASpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Team")
	FName TeamName;
	
};
