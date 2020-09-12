// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitMovementComponent.h"
#include "GameFramework/Character.h"
#include "DrawDebugHelpers.h"
#include "CollisionQueryParams.h"


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
	// TODO: rethink this
	//bIsShoved = false;
}


void UUnitMovementComponent::MoveThroughLane()
{
	if (!bIsAtFinalDestination)
	{
		MoveToDestination(TargetDestination);
		if (HasArrivedAtDestination(TargetDestination))
		{
			ProgressNextDestination();
		}
	}
	// TODO: rethink this
	//bIsShoved = false;
}


void UUnitMovementComponent::MoveToDestination(const FVector& Destination)
{
	if (IsValid(_OwningCharacter))
	{
		FVector currentLocation = _OwningCharacter->GetActorLocation();
		CheckIsStuck(currentLocation);

		FVector targetDirection = Destination - currentLocation;
		targetDirection.Normalize(SMALL_NUMBER);
		if (bIsShoved)
		{
			if (HasArrivedAtDestination(ShoveDestination))
			{
				bIsShoved = false;
			}
			else
			{
				FVector shoveDirection = ShoveDestination - currentLocation;
				shoveDirection.Normalize(SMALL_NUMBER);
				targetDirection = (targetDirection + shoveDirection) * 0.5f;
			}
		}

		_OwningCharacter->SetActorRotation(FVector(targetDirection.X, targetDirection.Y, 0).Rotation());
		_OwningCharacter->AddMovementInput(targetDirection);
		LastLocation = currentLocation;
	}
}


void UUnitMovementComponent::MoveByShove()
{
	MoveToDestination(ShoveDestination);
	if (HasArrivedAtDestination(ShoveDestination))
	{
		bIsShoved = false;
	}
}


void UUnitMovementComponent::RequestShove(const FVector& InShoveDestination)
{
	if (!bIsShoved)
	{
		// No double shoves
		ShoveDestination = InShoveDestination;
		bIsShoved = true;
	}
}


bool UUnitMovementComponent::IsBeingShoved() const
{
	return bIsShoved;
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


bool UUnitMovementComponent::HasArrivedAtDestination(const FVector& Destination)
{
	return FVector::DistXY(Destination, GetOwner()->GetActorLocation()) < DestinationDistanceLimit;
}


void UUnitMovementComponent::CheckIsStuck(const FVector& CurrentLocation) const
{
	if (CurrentLocation == LastLocation)
	{
		// TODO: Remove debug
		UE_LOG(LogConquestMovement, Log, TEXT("DETECTED STUCK UNIT: %s"), *GetNameSafe(_OwningCharacter));
		DrawDebugSphere(GetWorld(), CurrentLocation + FVector(0.f, 0.f, 100.f), 10.f, 16, FColor::Red);
		ShoveNeighbours();
	}
}


void UUnitMovementComponent::ShoveNeighbours() const
{
	// find neighbours
	FVector currentLocation = _OwningCharacter->GetActorLocation();
	TArray<AActor*> neighbours = GetNeighbours();
	for (AActor* neighbour : neighbours)
	{
		UE_LOG(LogConquestMovement, Log, TEXT("NEIGHBOUR: %s\t%s"), *GetNameSafe(_OwningCharacter), *(neighbour->GetName()));
		DrawDebugLine(GetWorld(), currentLocation, neighbour->GetActorLocation(), FColor::Blue);
		FVector shoveDestination = GetShoveDestination(neighbour);
		UUnitMovementComponent* movementComponent = (UUnitMovementComponent*)neighbour->GetComponentByClass(UUnitMovementComponent::StaticClass());
		movementComponent->RequestShove(shoveDestination);
	}
	


// 	_OwningCharacter->GetOverlappingActors(overlappingCharacters, ACharacter::StaticClass());
// 	for (AActor* neighbour : overlappingCharacters)
// 	{
// 		UE_LOG(LogConquestMovement, Log, TEXT("NEIGHBOUR: %s\t%s"), *GetNameSafe(_OwningCharacter), *GetNameSafe(neighbour));
// 		FVector neighbourDirection = neighbour->GetActorLocation() - currentLocation;
// 		float neighbourYaw = neighbourDirection.Rotation().Yaw;
// 		//float headingAngle = neighbourDirection.HeadingAngle();
// 		if (FMath::Abs(neighbourYaw - currentDirection.Yaw) < 50.f)
// 		{
// 			// i guess we are facing ?
// 			DrawDebugLine(GetWorld(), currentLocation, neighbour->GetActorLocation(), FColor::Blue);
// 		}
//	}

	// filter by movement direction
	// get unitmovementcomponent
	
	// request a shove in direction
}


TArray<AActor*> UUnitMovementComponent::GetNeighbours() const
{
	FVector currentLocation = _OwningCharacter->GetActorLocation();
	FVector forwardVector = _OwningCharacter->GetActorForwardVector();

	FVector right = forwardVector.RotateAngleAxis(45.f, FVector::UpVector) * 50.f;
	FVector left = forwardVector.RotateAngleAxis(-45.f, FVector::UpVector) * 50.f;
	FVector front = forwardVector * 50.f;


	// Trace complex?
	FCollisionQueryParams traceParams = FCollisionQueryParams(FName(TEXT("ShoveTrace")), false, _OwningCharacter);

	TArray<AActor*> neighbours;
	FHitResult hitResultRight;
	bool hitRight = GetWorld()->LineTraceSingleByChannel(hitResultRight, currentLocation, currentLocation + right, ECC_Pawn, traceParams);
	if (hitRight)
	{
		neighbours.Add(hitResultRight.GetActor());
	}
	FHitResult hitResultLeft;
	bool hitLeft = GetWorld()->LineTraceSingleByChannel(hitResultLeft, currentLocation, currentLocation + left, ECC_Pawn, traceParams);
	if (hitLeft)
	{
		neighbours.Add(hitResultLeft.GetActor());
	}
	FHitResult hitResultFront;
	bool hitFront = GetWorld()->LineTraceSingleByChannel(hitResultFront, currentLocation, currentLocation + front, ECC_Pawn, traceParams);
	if (hitFront)
	{
		neighbours.Add(hitResultFront.GetActor());
	}
	return neighbours;
}


FVector UUnitMovementComponent::GetShoveDestination(AActor* ActorToShove) const
{
	FVector actorLocation = ActorToShove->GetActorLocation();
	FVector shoveDirection = actorLocation - _OwningCharacter->GetActorLocation();
	shoveDirection.Normalize(SMALL_NUMBER);
	return actorLocation + shoveDirection * 60.f;
	
// 	FVector actorLocation = ActorToShove->GetActorLocation();
// 	FVector currentLocation = _OwningCharacter->GetActorLocation();
// 	FVector shoveDirection;
// 	if (actorLocation.Y < currentLocation.Y)
// 	{
// 		shoveDirection = FVector::LeftVector;
// 	}
// 	else
// 	{
// 		shoveDirection = FVector::RightVector;
// 	}
// 	return actorLocation + shoveDirection * 60.f;
}
