// Fill out your copyright notice in the Description page of Project Settings.

#include "SpawnPoint.h"

void AUnitSlot::SetIsOccupied(bool NewValue)
{
	bIsOccupied = NewValue;
}

bool AUnitSlot::IsOccupied() const
{
	return bIsOccupied;
}
