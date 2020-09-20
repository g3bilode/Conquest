// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ConquestPlayerController.h"
#include "ConquestCharacter.h"
#include "../Conquest.h"
#include "../Barracks/Barracks.h"
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


bool AConquestPlayerController::MakePurchase_Validate(int32 Cost)
{
	// TODO: Fix this - This can fail since client doesn't check CanMakePurchase on server...
	//return CanMakePurchase(Cost);
	return true;
}


void AConquestPlayerController::MakePurchase_Implementation(int32 Cost)
{
	ConquestPlayerState->Gold -= Cost;
}


bool AConquestPlayerController::SpawnUnit_Validate(TSubclassOf<class AConquestUnit> UnitToSpawn, FVector Location, int32 TeamIndex, int32 LaneIndex, const TArray<FVector>& LaneDestinations)
{
	return true;
}


void AConquestPlayerController::SpawnUnit_Implementation(TSubclassOf<class AConquestUnit> UnitToSpawn, FVector Location, int32 TeamIndex, int32 LaneIndex, const TArray<FVector>& LaneDestinations)
{
	FTransform spawnTransform;
	spawnTransform.SetLocation(Location);

	AActor* spawnedUnit = UGameplayStatics::BeginDeferredActorSpawnFromClass(GetWorld(), UnitToSpawn->GetDefaultObject()->GetClass(), spawnTransform, ESpawnActorCollisionHandlingMethod::DontSpawnIfColliding, this);
	AConquestUnit* conquestSpawnedUnit = Cast<AConquestUnit>(spawnedUnit);

	if (IsValid(conquestSpawnedUnit))
	{
		conquestSpawnedUnit->TeamIndex = TeamIndex;
		conquestSpawnedUnit->LaneIndex = LaneIndex;
		conquestSpawnedUnit->SetLaneDestinations(LaneDestinations);

		UGameplayStatics::FinishSpawningActor(conquestSpawnedUnit, spawnTransform);
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
	DECLARE_DELEGATE_OneParam(FInputDelegateOneParam, const bool);
	InputComponent->BindAction<FInputDelegateOneParam>("Select", EInputEvent::IE_Pressed, this, &AConquestPlayerController::OnClick, false);
	InputComponent->BindAction<FInputDelegateOneParam>("ShiftSelect", EInputEvent::IE_Pressed, this, &AConquestPlayerController::OnClick, true);

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


void AConquestPlayerController::OnClick(bool WithShift)
{
	if (!lookAroundEnabled)
	{
		if (bIsSpawningMode)
		{
			// TODO: Validate purchase on server
			if (CanMakePurchase(ActiveSpawner->PurchaseCost))
			{
				bool bWasSuccess = false;
				TSubclassOf<AUnitSpawner> activeSpawnerClass = ActiveSpawner->GetClass();
				if (ActiveSpawner->AttemptPurchase())
				{
					// TODO: Validate success on server
					MakePurchase(ActiveSpawner->PurchaseCost);
					bWasSuccess = true;
				}
				// Done with this spawn, on either success/fail
				ActiveSpawner = nullptr;
				bIsSpawningMode = false;
				// Continue with new spawning ?
				if (bWasSuccess && WithShift)
				{
					EnableSpawningMode(activeSpawnerClass);
				}
			}
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
	BuildBarracksArray();
}


bool AConquestPlayerController::CanMakePurchase(int32 Cost) const
{
	return Cost <= ConquestPlayerState->Gold;
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
		ActiveSpawner = (AUnitSpawner*)GetWorld()->SpawnActor(SpawnerClass->GetDefaultObject()->GetClass(), &FVector::ZeroVector);
		ActiveSpawner->TeamIndex = ConquestPlayerState->TeamIndex;
	}
}
