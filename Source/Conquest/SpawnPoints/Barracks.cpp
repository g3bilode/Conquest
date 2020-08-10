// Fill out your copyright notice in the Description page of Project Settings.


#include "Barracks.h"
#include "Conquest.h"
#include "SpawnPoint.h"

// Sets default values
ABarracks::ABarracks()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}


// Called when the game starts or when spawned
void ABarracks::BeginPlay()
{
	Super::BeginPlay();
	
}


ASpawnPoint* ABarracks::GetFreeUnitSlot()
{
	for (ASpawnPoint* unitSlot : UnitSlots)
	{
		if (!unitSlot->IsOccupied())
		{
			return unitSlot;
		}
	}
	return nullptr;
}


bool ABarracks::OnSelectionChanged_Implementation(AConquestPlayerController* initiator, AActor* NewSelection)
{
	return true;
}


bool ABarracks::OnSelectionGained_Implementation(AConquestPlayerController* initiator)
{
	UE_LOG(LogConquest, Log, TEXT("Barrack selected."));
	return true;
}
