// Fill out your copyright notice in the Description page of Project Settings.

#include "ConquestUnit.h"
#include "Conquest.h"
#include "UnrealNetwork.h"


// Sets default values
AConquestUnit::AConquestUnit()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CurrentDestinationIndex = -1;
}

void AConquestUnit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AConquestUnit, TeamName);
	DOREPLIFETIME(AConquestUnit, TargetDestination);
}

// Called when the game starts or when spawned
void AConquestUnit::BeginPlay()
{
	Super::BeginPlay();

	ProgressNextDestination();
}

// Called every frame
void AConquestUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if (!IsAtDestination)
	{
		MoveToDestination();
	}
}

// Called to bind functionality to input
void AConquestUnit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

FVector AConquestUnit::GetTargetDestination() const
{
	return TargetDestination;
}

void AConquestUnit::ProgressNextDestination()
{
	CurrentDestinationIndex++;
	if (CurrentDestinationIndex < LaneDestinations.Num())
	{
		SetTargetDestination(LaneDestinations[CurrentDestinationIndex]);
	}
	else
	{
		UE_LOG(LogConquest, Log, TEXT("I have arrived at final destination!"));
	}
}

void AConquestUnit::SetTargetDestination(FVector newLocation)
{
	TargetDestination = newLocation;
	IsAtDestination = false;
}

void AConquestUnit::SetLaneDestinations(const TArray<FVector>& InLaneDestinations)
{
	LaneDestinations = InLaneDestinations;
}

void AConquestUnit::MoveToDestination()
{
	FVector targetDirection = TargetDestination - GetActorLocation();
	targetDirection.Normalize(SMALL_NUMBER);

	SetActorRotation(FVector(targetDirection.X, targetDirection.Y, 0).Rotation());
	AddMovementInput(targetDirection);
	if (HasArrivedAtDestination())
	{
		UE_LOG(LogConquest, Log, TEXT("I have arrived!"));
		IsAtDestination = true;
		ProgressNextDestination();
	}
}

bool AConquestUnit::HasArrivedAtDestination()
{
	return FVector::DistXY(TargetDestination, GetActorLocation()) < 1.0f;
}
