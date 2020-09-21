// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitSpawner.h"
#include "../ConquestUnit.h"
#include "../../Barracks/Barracks.h"
#include "../../GameState/ConquestGameState.h"
#include "../../HUD/ConquestHUD.h"
#include "../../PlayerCharacter/ConquestPlayerController.h"


DEFINE_LOG_CATEGORY_STATIC(LogConquestUnitSpawner, Log, All);


// Sets default values
AUnitSpawner::AUnitSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
}


void AUnitSpawner::BeginPlay()
{
	Super::BeginPlay();

	UWorld* world = GetWorld();

	// Store reference to local player controller
	LocalPlayerController = Cast<AConquestPlayerController>(world->GetFirstPlayerController());

	// Bind to resource phase end
	AConquestGameState* conquestGameState = Cast<AConquestGameState>(world->GetGameState());
	if (IsValid(conquestGameState))
	{
		conquestGameState->ResourcePhase_OnEnd.AddDynamic(this, &AUnitSpawner::RespondToCombatPhaseBegin);
	}

}


void AUnitSpawner::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	Icon = UnitClass->GetDefaultObject<AConquestUnit>()->Icon;
	PurchaseCost = UnitClass->GetDefaultObject<AConquestUnit>()->PurchaseCost;
}


void AUnitSpawner::UpdatePosition(APlayerController* LocalController)
{
	bool onFriendlyBarrack = false;
	FHitResult HitResult;
	// Reuse Camera collision channel. Maybe should create custom collider instead?
	LocalController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Camera), false, HitResult);
	FVector location = HitResult.Location;
	AActor* const hitActor = HitResult.GetActor();
	if (IsValid(hitActor))
	{
		// Hit something
		if (hitActor->GetClass()->IsChildOf(ABarracks::StaticClass()))
		{
			// Hit Barrack! Snap to grid if friendly
			ABarracks* barracks = Cast<ABarracks>(hitActor);
			if (TeamIndex == barracks->TeamIndex)
			{
				ActiveBarracks = barracks;
				location = ActiveBarracks->GetNearestFreeSlotLocation(location);
				onFriendlyBarrack = true;
			}
		}
	}
	if (IsValid(ActiveBarracks) && !onFriendlyBarrack)
	{
		ActiveBarracks = nullptr;
	}

	SetActorLocation(location);
}


bool AUnitSpawner::AttemptPurchase()
{
	bool isSuccess = false;
	if (IsValid(ActiveBarracks))
	{
		int32 slotId = ActiveBarracks->GetSlotIDFromLocation(GetActorLocation());
		isSuccess = ActiveBarracks->OccupySlot(slotId);
	}
	if (!isSuccess)
	{
		// Purchase Fail
		Destroy();
	}
	else
	{
		// Purchase Success
		RefreshHUD(true);
		LaneIndex = ActiveBarracks->LaneIndex;
		LaneDestinations = ActiveBarracks->GetLaneDestinations();
		bIsActive = true;
	}
	return isSuccess;
}


bool AUnitSpawner::AttemptEvolve(int32 EvolutionIndex)
{
	bool isSuccess = false;
	TArray<TSubclassOf<class AConquestUnit>> evolutions = GetUnitEvolutions();
	if (evolutions.IsValidIndex(EvolutionIndex))
	{
		TSubclassOf<AConquestUnit> evolution = evolutions[EvolutionIndex];
		int32 evolutionCost = evolution->GetDefaultObject<AConquestUnit>()->PurchaseCost;
		if (LocalPlayerController->CanMakePurchase(evolutionCost))
		{
			LocalPlayerController->MakePurchase(evolutionCost);
			UnitClass = evolution;
			RefreshHUD(true);
			isSuccess = true;
		}
	}
	return isSuccess;
}


TArray<TSubclassOf<class AConquestUnit>> AUnitSpawner::GetUnitEvolutions()
{
	return UnitClass->GetDefaultObject<AConquestUnit>()->Evolutions;
}


void AUnitSpawner::RefreshHUD(bool IsSelected)
{
	AConquestHUD* conquestHUD = (AConquestHUD*)LocalPlayerController->GetHUD();
	if (IsSelected)
	{
		conquestHUD->OnSpawnerSelected(this);
	}
	else
	{
		conquestHUD->OnSpawnerDeselected();
	}
}


void AUnitSpawner::RespondToCombatPhaseBegin()
{
	if (bIsActive)
	{
		LocalPlayerController->SpawnUnit(UnitClass, GetActorLocation() + FVector(0, 0, 200), TeamIndex, LaneIndex, LaneDestinations);
	}
}


bool AUnitSpawner::OnSelectionChanged_Implementation(AConquestPlayerController* initiator, AActor* NewSelection)
{
	RefreshHUD(false);
	return true;
}


bool AUnitSpawner::OnSelectionGained_Implementation(AConquestPlayerController* initiator)
{
	if (initiator->GetConquestPlayerState()->TeamIndex == TeamIndex)
	{
		// Friendly
		RefreshHUD(true);
	}
	return true;
}
