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


	void OnRegister() override;

public:
	/* Move towards enemy. */
	void MoveToEnemy(const FVector& Destination);

	/* Move through lane. */
	void MoveThroughLane();

	/* Move to given destination. */
	void MoveToDestination(const FVector& Destination);

	/* Move by shove */
	void MoveByShove();

	/* Request a shove to given destination. */
	void RequestShove(const FVector& InShoveDestination);

	/* Return true if we are being shoved. */
	bool IsBeingShoved() const;

	/* Return True if has arrived at Destination. */
	bool HasArrivedAtDestination(const FVector& Destination);

	/* Set LaneDestinations. */
	void SetLaneDestinations(const TArray<FVector>& InLaneDestinations);


private:
	/* Current target destination */
	FVector TargetDestination;

	/* Shove destination */
	FVector ShoveDestination;

	/* Is being shoved. */
	bool bIsShoved;

	/* Last Location, to check if stuck. */
	FVector LastLocation;

	/* Is at destination? */
	bool bIsAtFinalDestination;

	/* List of destinations for unit's lane */
	TArray<FVector> LaneDestinations;

	/* Current location in lane */
	int8 CurrentDestinationIndex;

	static const float DestinationDistanceLimit;

	/* Component owner */
	ACharacter* _OwningCharacter;


	/* Set Target Destination. */
	void SetTargetDestination(FVector newLocation);

	/* Progress to the next destination. */
	void ProgressNextDestination();

	/* Return True if we are stuck during movement. */
	void CheckIsStuck(const FVector& CurrentLocation) const;

	/* When stuck, attempt a friendly shove. */
	void ShoveNeighbours() const;

	TArray<AActor*> GetNeighbours() const;

	/* Return FVector destination of the shove. */
	FVector GetShoveDestination(AActor* ActorToShove) const;

};
