// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interfaces/ConquestSelectableInterface.h"
#include "Outpost.generated.h"

UCLASS()
class CONQUEST_API AOutpost : public AActor, public IConquestSelectableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AOutpost();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Team")
	FName TeamName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stat")
	float MaxHealth;

	UPROPERTY(BlueprintReadWrite, Category = "Stat")
	float CurrentHealth;

private:
	virtual bool OnSelectionGained_Implementation(AConquestPlayerController* initiator) override;

	virtual bool OnSelectionChanged_Implementation(AConquestPlayerController* initiator, AActor* NewSelection) override;
	
};
