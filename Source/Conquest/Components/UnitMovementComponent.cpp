// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitMovementComponent.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"


DEFINE_LOG_CATEGORY_STATIC(LogConquestMovement, Log, All);

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
		CheckIsStuck(currentLocation);

		FVector targetDirection = Destination - currentLocation;
		targetDirection.Normalize(SMALL_NUMBER);

		_OwningCharacter->SetActorRotation(FVector(targetDirection.X, targetDirection.Y, 0).Rotation());
		_OwningCharacter->AddMovementInput(targetDirection);
		LastLocation = currentLocation;
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


void UUnitMovementComponent::CheckIsStuck(const FVector& CurrentLocation) const
{
	if (CurrentLocation == LastLocation)
	{
		// TODO: Remove debug
		UE_LOG(LogConquestMovement, Log, TEXT("DETECTED STUCK UNIT: %s"), *GetNameSafe(_OwningCharacter));
		DrawDebugSphere(GetWorld(), CurrentLocation + FVector(0.f, 0.f, 100.f), 10.f, 16, FColor::Red);
	}
}
