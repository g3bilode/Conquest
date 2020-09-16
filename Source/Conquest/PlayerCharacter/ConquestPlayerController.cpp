// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ConquestPlayerController.h"
#include "ConquestCharacter.h"
#include "../Conquest.h"
#include "../Barracks/Barracks.h"
#include "../Barracks/UnitSlot.h"
#include "../Capital/Capital.h"
#include "../CapturePoints/CapturePoint.h"
#include "../CheatManager/ConquestCheatManager.h"
#include "../Components/HealthComponent.h"
#include "../GameState/ConquestGameState.h"
#include "../Units/ConquestUnit.h"
#include "../Utils/ConquestUtils.h"
#include "../Units/Spawners/UnitSpawner.h"
#include "Algo/Reverse.h"
#include "EngineUtils.h"
#include "Engine/LocalPlayer.h"
#include "Kismet/GameplayStatics.h"
#include "Net/UnrealNetwork.h"

AConquestPlayerController::AConquestPlayerController()
{
	bShowMouseCursor = true;
	CheatClass = UConquestCheatManager::StaticClass();
}

// Called when the game starts or when spawned
void AConquestPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ConquestPlayerState = Cast<AConquestPlayerState>(PlayerState);
	if (HasAuthority())
	{
		OnRep_ConquestPlayerState();
	}
}

void AConquestPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AConquestPlayerController, ConquestPlayerState, COND_InitialOnly);
}

bool AConquestPlayerController::AttemptSpawnUnit_Validate(TSubclassOf<class AConquestUnit> ActorToSpawn, int32 LaneIndex, int32 SlotIndex)
{
	return true;
}

void AConquestPlayerController::AttemptSpawnUnit_Implementation(TSubclassOf<class AConquestUnit> ActorToSpawn, int32 LaneIndex, int32 SlotIndex)
{
	if (!CanPurchaseUnit(ActorToSpawn))
	{
		UE_LOG(LogConquest, Log, TEXT("Cannot purchase unit."));
		return;
	}

	ABarracks* laneBarracks = GetBarracksForLane(LaneIndex);
	if (laneBarracks->SpawnUnitAtIndex(ActorToSpawn, SlotIndex))
	{
		PurchaseUnit(ActorToSpawn);
	}
}

void AConquestPlayerController::SetupInputComponent()
{
	// set up gameplay key bindings
	Super::SetupInputComponent();

	InputComponent->BindAxis("MoveForward", this, &AConquestPlayerController::OnMoveForwardAxis);
	InputComponent->BindAxis("MoveRight", this, &AConquestPlayerController::OnMoveRightAxis);
	InputComponent->BindAxis("MouseHorizontal", this, &AConquestPlayerController::OnMouseHorizontal);
	InputComponent->BindAxis("MouseVertical", this, &AConquestPlayerController::OnMouseVertical);
	InputComponent->BindAction("ZoomIn", EInputEvent::IE_Pressed, this, &AConquestPlayerController::OnZoomInAction);
	InputComponent->BindAction("ZoomOut", EInputEvent::IE_Pressed, this, &AConquestPlayerController::OnZoomOutAction);
	InputComponent->BindAction("LookAround", EInputEvent::IE_Pressed, this, &AConquestPlayerController::OnLookAroundStart);
	InputComponent->BindAction("LookAround", EInputEvent::IE_Released, this, &AConquestPlayerController::OnLookAroundStop);
	InputComponent->BindAction("Select", EInputEvent::IE_Pressed, this, &AConquestPlayerController::OnSelect);
}

void AConquestPlayerController::OnMoveForwardAxis(float axisValue)
{
	APawn* const pawn = GetPawn();
	AConquestCharacter* character = Cast<AConquestCharacter>(pawn);
	if (character)
	{
		character->MoveCharacterForward(axisValue);
	}
}

void AConquestPlayerController::OnMoveRightAxis(float axisValue)
{
	APawn* const pawn = GetPawn();
	AConquestCharacter* character = Cast<AConquestCharacter>(pawn);
	if (character)
	{
		character->MoveCharacterRight(axisValue);
	}
}

void AConquestPlayerController::OnMouseHorizontal(float axisValue)
{
	if (lookAroundEnabled)
	{
		APawn* const pawn = GetPawn();
		pawn->AddControllerYawInput(axisValue);
		Cast<ULocalPlayer>(Player)->ViewportClient->Viewport->SetMouse(mouseLockPositionX, mouseLockPositionY);
	}
	UpdateSpawnerPosition();
}

void AConquestPlayerController::OnMouseVertical(float axisValue)
{
	if (lookAroundEnabled)
	{
		APawn* const pawn = GetPawn();
		AConquestCharacter* character = Cast<AConquestCharacter>(pawn);
		if (character)
		{
			character->RotateCameraArm(FRotator(axisValue, 0.0f, 0.0f));
		}
		Cast<ULocalPlayer>(Player)->ViewportClient->Viewport->SetMouse(mouseLockPositionX, mouseLockPositionY);
	}
	UpdateSpawnerPosition();
}

void AConquestPlayerController::OnZoomInAction()
{
	APawn* const pawn = GetPawn();
	AConquestCharacter* character = Cast<AConquestCharacter>(pawn);
	if (character)
	{
		character->ChangeCameraArmLength(-1.0f);
	}
}

void AConquestPlayerController::OnZoomOutAction()
{
	APawn* const pawn = GetPawn();
	AConquestCharacter* character = Cast<AConquestCharacter>(pawn);
	if (character)
	{
		character->ChangeCameraArmLength(1.0f);
	}
}

void AConquestPlayerController::OnLookAroundStart()
{
	//Lock mouse cursor
	lookAroundEnabled = true;
	bShowMouseCursor = false;
	mouseLockPositionX = Cast<ULocalPlayer>(Player)->ViewportClient->Viewport->GetMouseX();
	mouseLockPositionY = Cast<ULocalPlayer>(Player)->ViewportClient->Viewport->GetMouseY();
}

void AConquestPlayerController::OnLookAroundStop()
{
	//Unlock mouse cursor
	lookAroundEnabled = false;
	bShowMouseCursor = true;
}

void AConquestPlayerController::OnSelect()
{
	if (!lookAroundEnabled)
	{
		if (bIsSpawningMode)
		{
			// TODO: Validate purchase on server
			ActiveSpawner->AttemptPurchase();
			ActiveSpawner = nullptr;
			// TODO: SHIFT to spawn a new spawner
			bIsSpawningMode = false;
			return;
		}

		FHitResult HitResult;
		GetHitResultUnderCursorByChannel(UEngineTypes::ConvertToTraceType(ECC_Visibility), true, HitResult);
		AActor* const NewSelection = HitResult.GetActor();

		if (IsValid(NewSelection))
		{
			// Previous Selection action
			if (IsValid(SelectedActor.Get()))
			{
				IConquestSelectableInterface::Execute_OnSelectionChanged(SelectedActor.Get(), this, NewSelection);
			}

			// Selection action
			if (NewSelection->GetClass()->ImplementsInterface(UConquestSelectableInterface::StaticClass()))
			{
				IConquestSelectableInterface::Execute_OnSelectionGained(NewSelection, this);
				SelectedActor = NewSelection;
			}
		}
		
		// Character action
		APawn* const pawn = GetPawn();
		AConquestCharacter* const character = Cast<AConquestCharacter>(pawn);
		if (character)
		{
			character->OnSelectActor(SelectedActor.Get());
		}
	}
}


void AConquestPlayerController::OnRep_ConquestPlayerState()
{
	GatherCapitals();
}


bool AConquestPlayerController::CanPurchaseUnit(const TSubclassOf<class AConquestUnit> unit) const
{
	int32 cost = unit->GetDefaultObject<AConquestUnit>()->PurchaseCost;
	return cost <= ConquestPlayerState->Gold;
}


void AConquestPlayerController::PurchaseUnit(const TSubclassOf<class AConquestUnit> unit)
{
	int32 cost = unit->GetDefaultObject<AConquestUnit>()->PurchaseCost;
	ConquestPlayerState->Gold -= cost;
}


void AConquestPlayerController::BuildLaneArray()
{
	// TODO: Improve this? Move this?
	// Init the LaneArray
	int32 numLanes = ConquestUtils::LaneSpec.Num();
	LaneArray.Init(FLaneDestinations(), numLanes);
	int8 idx = 0;
	for (FLaneDestinations& lane : LaneArray)
	{
		lane.LaneDestinations.Init(FVector(), ConquestUtils::LaneSpec[idx]);
		idx++;
	}

	// Assign LaneArray values
	for (TActorIterator<ACapturePoint> CapturePointItr(GetWorld()); CapturePointItr; ++CapturePointItr)
	{
		int8 LaneNumber = CapturePointItr->GetLaneNumber();
		int8 RowNumber = CapturePointItr->GetRowNumber();
		LaneArray[LaneNumber].LaneDestinations[RowNumber] = CapturePointItr->GetActorLocation();
	}

	// Get the final destination: the enemy base
	FVector enemyLocation;
	for (TActorIterator<ACapital> CapitalItr(GetWorld()); CapitalItr; ++CapitalItr)
	{
		if (CapitalItr->TeamIndex != ConquestPlayerState->TeamIndex)
		{
			enemyLocation = CapitalItr->GetActorLocation();
			break;
		}
	}

	// Final touches
	for (FLaneDestinations& lane : LaneArray)
	{
		if (ConquestPlayerState->TeamIndex > 0)
		{
			// Not first player, flip rows
			Algo::Reverse(lane.LaneDestinations);
		}
		// Add enemy base location
		lane.LaneDestinations.Add(enemyLocation);
	}
	if (ConquestPlayerState->TeamIndex > 0)
	{
		// Not first player, flip lanes
		Algo::Reverse(LaneArray);
	}
}

void AConquestPlayerController::BuildBarracksArray()
{
	int32 numBarracks = ConquestUtils::LaneSpec.Num();
	BarracksArray.Init(nullptr, numBarracks);

	// Assign BarrackArray values
	for (TActorIterator<ABarracks> BarracksItr(GetWorld()); BarracksItr; ++BarracksItr)
	{
		if (BarracksItr->TeamIndex == ConquestPlayerState->TeamIndex)
		{
			int32 LaneIndex = BarracksItr->LaneIndex;
			BarracksItr->SetLaneDestinations(GetLaneDestinations(LaneIndex));
			BarracksArray[LaneIndex] = *BarracksItr;
		}
	}

}

void AConquestPlayerController::GatherCapitals()
{
	for (TActorIterator<ACapital> CapitalItr(GetWorld()); CapitalItr; ++CapitalItr)
	{
		if (CapitalItr->TeamIndex == ConquestPlayerState->TeamIndex)
		{
			FriendlyCapital = *CapitalItr;
		}
		else
		{
			EnemyCapital = *CapitalItr;
		}
	}
}


void AConquestPlayerController::UpdateSpawnerPosition()
{
	if (bIsSpawningMode && IsValid(ActiveSpawner))
	{
		ActiveSpawner->UpdatePosition(this);
	}
}


AConquestPlayerState* AConquestPlayerController::GetConquestPlayerState()
{
	return ConquestPlayerState;
}

TArray<FVector> AConquestPlayerController::GetLaneDestinations(int32 Index)
{
	if (LaneArray.Num() == 0)
	{
		BuildLaneArray();
	}
	return LaneArray[Index].LaneDestinations;
}

class ABarracks* AConquestPlayerController::GetBarracksForLane(int32 LaneIndex)
{
	if (BarracksArray.Num() == 0)
	{
		BuildBarracksArray();
	}
	return BarracksArray[LaneIndex];
}

float AConquestPlayerController::GetFriendlyCapitalHealthPercent()
{
	if (IsValid(FriendlyCapital))
	{
		UHealthComponent* capitalHealthComponent = (UHealthComponent*)FriendlyCapital->GetComponentByClass(UHealthComponent::StaticClass());
		return capitalHealthComponent->GetHealthPercent();
	}
	return 0.0f;
}


void AConquestPlayerController::EnableSpawningMode(TSubclassOf<class AUnitSpawner> SpawnerClass)
{
	bIsSpawningMode = true;
	if (IsValid(SpawnerClass))
	{
		ActiveSpawner = (AUnitSpawner*) GetWorld()->SpawnActor(SpawnerClass->GetDefaultObject()->GetClass(), &FVector::ZeroVector );
		ActiveSpawner->TeamIndex = ConquestPlayerState->TeamIndex;
	}
}
