// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitSpawner.h"
#include "../../Barracks/Barracks.h"

// Sets default values
AUnitSpawner::AUnitSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}


void AUnitSpawner::UpdatePosition(APlayerController* LocalController)
{
	FHitResult HitResult;
	LocalController->GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), false, HitResult);
	FVector location = HitResult.Location;
	AActor* const hitActor = HitResult.GetActor();
	if (IsValid(hitActor))
	{
		// Hit something
		if (hitActor->GetClass()->IsChildOf(ABarracks::StaticClass()))
		{
			// Hit Barrack! Snap to grid
			ABarracks* hitBarracks = Cast<ABarracks>(hitActor);
			location = hitBarracks->GetNearestFreeSlotLocation(location);
		}
	}


	SetActorLocation(location);
}

