// Fill out your copyright notice in the Description page of Project Settings.

#include "CapturePoint.h"
#include "Conquest.h"


bool ACapturePoint::OnSelectionGained_Implementation()
{
	UE_LOG(LogConquest, Log, TEXT("I am Selected! (%s)"), *GetNameSafe(this));
	return true;
}

bool ACapturePoint::OnSelectionChanged_Implementation(AConquestPlayerController* initiator, AActor* NewSelection)
{
	return true;
}