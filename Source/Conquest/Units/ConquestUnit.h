// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ConquestUnit.generated.h"

UCLASS()
class CONQUEST_API AConquestUnit : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AConquestUnit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	FVector GetTargetDestination() const;

	void ProgressNextDestination();

	void SetTargetDestination(FVector newLocation);

	void SetLaneDestinations(const TArray<FVector>& InLaneDestinations);

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Team")
	FName TeamName;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cost")
	int32 PurchaseCost;
	
private:

	void MoveToDestination();
	bool HasArrivedAtDestination();

	UPROPERTY(Replicated)
	FVector TargetDestination;
	UPROPERTY()
	bool IsAtDestination;
	/* List of destinations for unit's lane */
	TArray<FVector> LaneDestinations;
	/* Current location in lane */
	int8 CurrentDestinationIndex;
};
