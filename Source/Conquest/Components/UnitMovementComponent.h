// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UnitMovementComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CONQUEST_API UUnitMovementComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UUnitMovementComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	/* Move towards enemy. */
	void MoveToEnemy(const FVector& Destination);

	/* Move through lane. */
	void MoveThroughLane();

	/* Move to given destination. */
	void MoveToDestination(const FVector& Destination);

	/* Return True if has arrived at destination. */
	bool HasArrivedAtDestination();

	/* Set LaneDestinations. */
	void SetLaneDestinations(const TArray<FVector>& InLaneDestinations);


private:
	/* Current target destination */
	FVector TargetDestination;

	/* Is at destination? */
	bool bIsAtFinalDestination;

	/* List of destinations for unit's lane */
	TArray<FVector> LaneDestinations;

	/* Current location in lane */
	int8 CurrentDestinationIndex;


	/* Set Target Destination. */
	void SetTargetDestination(FVector newLocation);

	/* Progress to the next destination. */
	void ProgressNextDestination();

};