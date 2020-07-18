// Fill out your copyright notice in the Description page of Project Settings.

#include "ConquestUnit.h"
#include "Conquest.h"
#include "Outpost/Outpost.h"
#include "PlayerCharacter/ConquestPlayerController.h"
#include "PlayerCharacter/ConquestPlayerState.h"
#include "UnrealNetwork.h"
#include "CapturePoints/CapturePoint.h"


// Sets default values
AConquestUnit::AConquestUnit()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

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
	
}

// Called every frame
void AConquestUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

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

void AConquestUnit::SetTargetDestination(FVector newLocation)
{
	TargetDestination = newLocation;
}

bool AConquestUnit::OnSelectionGained_Implementation(AConquestPlayerController* initiator)
{
	UE_LOG(LogConquest, Log, TEXT("I am Selected! (%s)"), *GetNameSafe(this));
	return true;
}

bool AConquestUnit::OnSelectionChanged_Implementation(AConquestPlayerController* initiator, AActor* NewSelection)
{
	AConquestPlayerState* playerState = Cast<AConquestPlayerState>(initiator->PlayerState);
	if (playerState != nullptr)
	{
		if (playerState->TeamName != TeamName)
		{
			UE_LOG(LogConquest, Log, TEXT("Cant touch me!: %s"), *(playerState->TeamName.ToString()));
			return false;
		}
	}
	// OUTPOST
	if (NewSelection->GetClass()->IsChildOf(AOutpost::StaticClass()))
	{
		UE_LOG(LogConquest, Log, TEXT("New Target Location!"));
		initiator->MoveUnit(this, NewSelection->GetActorLocation());
	}
	// CAPTURE POINT
	if (NewSelection->GetClass()->IsChildOf(ACapturePoint::StaticClass()))
	{
		UE_LOG(LogConquest, Log, TEXT("Move to CapturePoint!"));
		ACapturePoint* capturePoint = Cast<ACapturePoint>(NewSelection);
		const FVector* destination = capturePoint->getDestinationForUnit(TeamName);
		if (destination != nullptr)
		{
			initiator->MoveUnit(this, *destination);
		}
	}
	return true;
}