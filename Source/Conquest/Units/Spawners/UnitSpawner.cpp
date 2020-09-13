// Fill out your copyright notice in the Description page of Project Settings.


#include "UnitSpawner.h"

// Sets default values
AUnitSpawner::AUnitSpawner()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}


void AUnitSpawner::UpdatePosition(FVector TargetLocation)
{
	SetActorLocation(TargetLocation);
}

