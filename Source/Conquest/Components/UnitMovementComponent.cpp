// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitMovementComponent.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h" //remove this on refactor
#include "../Units/ConquestUnit.h" //remove this on refactor


DEFINE_LOG_CATEGORY_STATIC(LogConquestMovement, Log, All);

const float UUnitMovementComponent::DestinationDistanceLimit = 50.0f;


// Sets default values for this component's properties
UUnitMovementComponent::UUnitMovementComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	CurrentDestinationIndex = -1;

	TargetDirectionWeight = 10.f;
	AvoidanceWeight = 5.f;
	AvoidanceDistanceLimit = 60.f;
}


// Called when the game starts
void UUnitMovementComponent::BeginPlay()
{
	Super::BeginPlay();

	ProgressNextDestination();	
}


void UUnitMovementComponent::OnRegister()
{
	Super::OnRegister();
	_OwningCharacter = (ACharacter*)GetOwner();
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
	if (IsValid(_OwningCharacter))
	{
		FVector currentLocation = _OwningCharacter->GetActorLocation();
		///CheckIsStuck(currentLocation);

		FVector targetDirection = Destination - currentLocation;
		targetDirection.Normalize();

		// get friends in range
		// get vector away * distance = avoidance vectors
		// do for each friend
		TArray<FVector> avoidanceVectors = GatherAvoidanceVectors(currentLocation);
		/// boost Y value ?

		// (target direction * target direction weight)+(avoidance vectors * avoidance weight)
		FVector finalDirection = targetDirection * TargetDirectionWeight;
		for (FVector avoidanceVector : avoidanceVectors)
		{
			finalDirection += avoidanceVector * AvoidanceWeight;
		}
		// divide by num avoidance +1
		finalDirection /= avoidanceVectors.Num() + 1;
		// normalize
		finalDirection.Normalize();

		/// Notify friend that he should move?

		_OwningCharacter->SetActorRotation(FVector(targetDirection.X, targetDirection.Y, 0).Rotation());
		///_OwningCharacter->AddMovementInput(targetDirection);
		_OwningCharacter->AddMovementInput(finalDirection);
		LastLocation = currentLocation;

		// TODO: remove debug
		UWorld* world = GetWorld();
		DrawDebugLine(world, currentLocation, currentLocation + targetDirection * 100, FColor::Blue);
		DrawDebugLine(world, currentLocation, currentLocation + finalDirection * 100, FColor::Green);
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


TArray<FVector> UUnitMovementComponent::GatherAvoidanceVectors(FVector CurrentLocation)
{
	// TODO: Improve performance
	TArray<FVector> avoidanceVectors;
	TArray<AActor*> conquestUnitActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), AConquestUnit::StaticClass(), conquestUnitActors);
	for (AActor* conquestUnitActor : conquestUnitActors)
	{
		float unitDistance = FVector::DistXY(CurrentLocation, conquestUnitActor->GetActorLocation());
		if ((0.f < unitDistance) && (unitDistance < AvoidanceDistanceLimit))
		{
			//FVector avoidanceVector = conquestUnitActor->GetActorLocation() - CurrentLocation;
			FVector avoidanceVector = CurrentLocation - conquestUnitActor->GetActorLocation();
			avoidanceVector.Normalize();
			float importance = 1.f; //AvoidanceDistanceLimit - unitDistance;
			avoidanceVectors.Add(avoidanceVector * importance);
		}
	}
	return avoidanceVectors;
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


void UUnitMovementComponent::CheckIsStuck(const FVector& CurrentLocation) const
{
	if (CurrentLocation == LastLocation)
	{
		// TODO: Remove debug
		UE_LOG(LogConquestMovement, Log, TEXT("DETECTED STUCK UNIT: %s"), *GetNameSafe(_OwningCharacter));
		DrawDebugSphere(GetWorld(), CurrentLocation + FVector(0.f, 0.f, 100.f), 10.f, 16, FColor::Red);
	}
}
