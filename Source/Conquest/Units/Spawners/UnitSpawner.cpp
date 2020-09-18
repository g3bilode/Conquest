// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitSpawner.h"
#include "../ConquestUnit.h"
#include "../../Barracks/Barracks.h"
#include "../../GameState/ConquestGameState.h"
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

	// Bind to combat phase start
	AConquestGameState* conquestGameState = (AConquestGameState*)GetWorld()->GetGameState();
	conquestGameState->ResourcePhase_OnEnd.AddDynamic(this, &AUnitSpawner::RespondToCombatPhaseBegin);
}


void AUnitSpawner::UpdatePosition(APlayerController* LocalController)
{
	bool onFriendlyBarrack = false;
	FHitResult HitResult;
	LocalController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), false, HitResult);
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
		LaneIndex = ActiveBarracks->LaneIndex;
		LaneDestinations = ActiveBarracks->GetLaneDestinations();
	}
	return isSuccess;
}


void AUnitSpawner::RespondToCombatPhaseBegin()
{
	UE_LOG(LogConquestUnitSpawner, Log, TEXT("COMBAT BEGIN: %s"), *GetNameSafe(this));
	AConquestPlayerController* playerController = (AConquestPlayerController*)GetWorld()->GetFirstPlayerController();
	playerController->SpawnUnit(UnitClass, GetActorLocation() + FVector(0, 0, 200), TeamIndex, LaneIndex, LaneDestinations);
}
