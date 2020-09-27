// Fill out your copyright notice in the Description page of Project Settings.

#include "CapturePoint.h"
#include "../Conquest.h"
#include "../GameState/ConquestGameState.h"
#include "../PlayerCharacter/ConquestPlayerState.h"
#include "../GameMode/ConquestGameMode.h"
#include "../HUD/ResourceDripComponent.h"
#include "Net/UnrealNetwork.h"


bool ACapturePoint::OnSelectionGained_Implementation(AConquestPlayerController* initiator)
{
	UE_LOG(LogConquest, Log, TEXT("I am Selected! (%s)"), *GetNameSafe(this));
	return true;
}


bool ACapturePoint::OnSelectionChanged_Implementation(AConquestPlayerController* initiator, AActor* NewSelection)
{
	return true;
}


ACapturePoint::ACapturePoint()
{
	OccupierTeamIndex = -1;

	// Setup root component
	RootComponent = CreateDefaultSubobject<USceneComponent>("RootComponent");
	// Setup ResourceDripComponent
	ResourceDripComponent = CreateDefaultSubobject<UResourceDripComponent>("ResourceDripComponent");
	ResourceDripComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
}


void ACapturePoint::Capture(int32 TeamIndex)
{
	OccupierTeamIndex = TeamIndex;
	DisplayCapture();
}


void ACapturePoint::DisplayGlintDrip(int32 GlintAmount)
{
	ResourceDripComponent->ActivateDripWidget(GlintAmount);
}


int8 ACapturePoint::GetLaneNumber() const
{
	return LaneNumber;
}


int8 ACapturePoint::GetRowNumber() const
{
	return RowNumber;
}


void ACapturePoint::BeginPlay()
{
	Super::BeginPlay();

	ResourceDripComponent->SetupWidget(FLinearColor(0.36f, 0.226f, 1.0f));
}


void ACapturePoint::OnTriggerOverlapWithUnit(int32 TeamIndex)
{
	if (TeamIndex != OccupierTeamIndex)
	{
		Capture(TeamIndex);
	}
}

