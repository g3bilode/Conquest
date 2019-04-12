// Fill out your copyright notice in the Description page of Project Settings.

#include "Outpost.h"
#include "Conquest.h"
#include "Kismet/GameplayStatics.h"
#include "PlayerCharacter/ConquestPlayerController.h"

// Sets default values
AOutpost::AOutpost()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

bool AOutpost::OnSelectionGained_Implementation(AConquestPlayerController* initiator)
{
	UE_LOG(LogConquest, Log, TEXT("Outpost Selected! (%s)"), *GetNameSafe(this));
	FVector location = AActor::GetActorLocation();
	FVector2D screenLocation;
	initiator->ProjectWorldLocationToScreen(location, screenLocation);
	initiator->SetOutpostMenuLocation(screenLocation);
	initiator->SetOutpostMenuVisibility(true);
	return true;
}

bool AOutpost::OnSelectionChanged_Implementation(AConquestPlayerController* initiator, AActor* NewSelection)
{
	initiator->SetOutpostMenuVisibility(false);
	return true;
}
