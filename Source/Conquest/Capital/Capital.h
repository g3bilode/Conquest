// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interfaces/ConquestSelectableInterface.h"
#include "../Interfaces/TargeterInterface.h"
#include "Capital.generated.h"

UCLASS()
class CONQUEST_API ACapital : public AActor, public IConquestSelectableInterface, public ITargeterInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACapital();

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Team")
	int32 TeamIndex;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Stat")
	float MaxHealth;

	UPROPERTY(BlueprintReadWrite, Category = "Stat")
	float CurrentHealth;

private:
	virtual bool OnSelectionGained_Implementation(AConquestPlayerController* initiator) override;

	virtual bool OnSelectionChanged_Implementation(AConquestPlayerController* initiator, AActor* NewSelection) override;

	UPROPERTY()
	/* Root component */
	class USceneComponent* RootSceneComponent;
	/* Attack Component*/
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	class UAttackComponent* AttackComponent;
	/* Targeting Component*/
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	class UTargetingComponent* TargetingComponent;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool IsTargetEnemy_Implementation(AActor* OtherActor) override;

};
