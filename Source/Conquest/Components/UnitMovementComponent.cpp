// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitMovementComponent.h"
#include "GameFramework/Character.h"


const float UUnitMovementComponent::DestinationDistanceLimit = 50.0f;


// Sets default values for this component's properties
UUnitMovementComponent::UUnitMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	CurrentDestinationIndex = -1;
}


// Called when the game starts
void UUnitMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	ProgressNextDestination();	
}


void UUnitMovementComponent::MoveToEnemy(const FVector& Destination)
{
	MoveToDestination(Destination);
}


void UUnitMovementComponent::MoveThroughLane()
{
	if (!bIsAtFinalDestination)
	{
		MoveToDestination(TargetDestination);
		if (HasArrivedAtDestination())
		{
			ProgressNextDestination();
		}
	}
}


void UUnitMovementComponent::MoveToDestination(const FVector& Destination)
{
	// TODO: If hasn't moved in a while, disable collision for a bit
	ACharacter* owningCharacter = (ACharacter*)GetOwner();
	if (IsValid(owningCharacter))
	{
		FVector targetDirection = Destination - owningCharacter->GetActorLocation();
		targetDirection.Normalize(SMALL_NUMBER);

		owningCharacter->SetActorRotation(FVector(targetDirection.X, targetDirection.Y, 0).Rotation());
		owningCharacter->AddMovementInput(targetDirection);
	}
}


void UUnitMovementComponent::ProgressNextDestination()
{
	CurrentDestinationIndex++;
	if (CurrentDestinationIndex < LaneDestinations.Num())
	{
		SetTargetDestination(LaneDestinations[CurrentDestinationIndex]);
	}
	else
	{
		bIsAtFinalDestination = true;
	}
}


void UUnitMovementComponent::SetLaneDestinations(const TArray<FVector>& InLaneDestinations)
{
	LaneDestinations = InLaneDestinations;
}


void UUnitMovementComponent::SetTargetDestination(FVector newLocation)
{
	TargetDestination = newLocation;
}


bool UUnitMovementComponent::HasArrivedAtDestination()
{
	return FVector::DistXY(TargetDestination, GetOwner()->GetActorLocation()) < DestinationDistanceLimit;
}
