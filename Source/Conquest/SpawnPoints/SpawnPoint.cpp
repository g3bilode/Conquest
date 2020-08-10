// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnPoint.h"

void ASpawnPoint::SetIsOccupied(bool NewValue)
{
	bIsOccupied = NewValue;
}

bool ASpawnPoint::IsOccupied() const
{
	return bIsOccupied;
}
