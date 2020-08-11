// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ConquestPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Conquest.h"
#include "ConquestCharacter.h"
#include "Engine/LocalPlayer.h"
#include "CapturePoints/CapturePoint.h"
#include "Units/ConquestUnit.h"
#include "Kismet/GameplayStatics.h"
#include "Barracks/UnitSlot.h"
#include "UnrealNetwork.h"
#include "EngineUtils.h"
#include "GameState/ConquestGameState.h"
#include "Outpost/Outpost.h"
#include "Algo/Reverse.h"
#include "Utils/ConquestUtils.h"
#include "Barracks/Barracks.h"

AConquestPlayerController::AConquestPlayerController()
{
	bShowMouseCursor = true;
	static ConstructorHelpers::FClassFinder<UUserWidget> outpostHud(TEXT("/Game/Conquest/UI/HUD/Outpost_HUD"));
	wOutpostMenu = outpostHud.Class;
}

// Called when the game starts or when spawned
void AConquestPlayerController::BeginPlay()
{
	Super::BeginPlay();

	ConquestPlayerState = Cast<AConquestPlayerState>(PlayerState);
	if (IsLocalPlayerController())
	{
		CreateUI();
	}
}

void AConquestPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AConquestPlayerController, ConquestPlayerState);
}

bool AConquestPlayerController::AttemptSpawnUnit_Validate(TSubclassOf<class AConquestUnit> ActorToSpawn, const TArray<FVector>& LaneDestinations, int32 LaneIndex)
{
	return true;
}

void AConquestPlayerController::AttemptSpawnUnit_Implementation(TSubclassOf<class AConquestUnit> ActorToSpawn, const TArray<FVector>& LaneDestinations, int32 LaneIndex)
{
	
	if (!CanPurchaseUnit(ActorToSpawn))
	{
		UE_LOG(LogConquest, Log, TEXT("Cannot purchase unit."));
		return;
	}

	FVector location;
	ABarracks* laneBarracks = GetBarracksForLane(LaneIndex);
	AUnitSlot* freeSlot = laneBarracks->GetFreeUnitSlot();
	if (IsValid(freeSlot))
	{
		location = freeSlot->GetActorLocation();
		// Avoid spawn collision...
		location += FVector(0, 0, 200);

	}
	else
	{
		UE_LOG(LogConquest, Error, TEXT("No free barracks slot."));
		return;
	}

	FTransform spawnTransform;
	spawnTransform.SetLocation(location);

	AActor* spawnedUnit = UGameplayStatics::BeginDeferredActorSpawnFromClass(this, ActorToSpawn->GetDefaultObject()->GetClass(), spawnTransform, ESpawnActorCollisionHandlingMethod::Undefined, this);
	AConquestUnit* conquestSpawnedUnit = Cast<AConquestUnit>(spawnedUnit);

	if (ConquestPlayerState != nullptr && IsValid(conquestSpawnedUnit))
	{
		conquestSpawnedUnit->TeamName = ConquestPlayerState->TeamName;
		conquestSpawnedUnit->TeamIndex = ConquestPlayerState->TeamIndex;
		conquestSpawnedUnit->LaneIndex = LaneIndex;
		conquestSpawnedUnit->SetLaneDestinations(LaneDestinations);

		UGameplayStatics::FinishSpawningActor(conquestSpawnedUnit, spawnTransform);
		if (IsValid(conquestSpawnedUnit))
		{
			PurchaseUnit(ActorToSpawn);
			freeSlot->SetIsOccupied(true);
		}
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
	// TODO: Improve this?
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
	for (TActorIterator<AOutpost> OutPostItr(GetWorld()); OutPostItr; ++OutPostItr)
	{
		if (OutPostItr->TeamName != ConquestPlayerState->TeamName)
		{
			enemyLocation = OutPostItr->GetActorLocation();
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
			BarracksArray[LaneIndex] = *BarracksItr;
		}
	}

}

void AConquestPlayerController::CreateUI()
{
	if (!_menuOutpost)
	{
		_menuOutpost = CreateWidget<UUserWidget>(this, wOutpostMenu, "OutpostMenu");
		_menuOutpost->AddToViewport();
		_menuOutpost->SetVisibility(ESlateVisibility::Hidden);
	}
}

AConquestPlayerState* AConquestPlayerController::GetConquestPlayerState()
{
	return ConquestPlayerState;
}

void AConquestPlayerController::OnOutpostSelect(AOutpost* outpost)
{
	_selectedOutpost = outpost;	
}

void AConquestPlayerController::DisplayOutpostMenu(const FVector& outpostLocation)
{
	FVector2D screenLocation;
	ProjectWorldLocationToScreen(outpostLocation, screenLocation);
	_menuOutpost->SetPositionInViewport(screenLocation);
	SetOutpostMenuVisibility(true);
}

void AConquestPlayerController::SetOutpostMenuVisibility(const bool isVisible) const
{
	if (IsValid(_menuOutpost))
	{
		if (isVisible)
		{
			_menuOutpost->SetVisibility(ESlateVisibility::Visible);
		}
		else {
			_menuOutpost->SetVisibility(ESlateVisibility::Hidden);
		}
	}
}

void AConquestPlayerController::AttackOutpost(AOutpost* outpost)
{
	UE_LOG(LogConquest, Log, TEXT("CHAAARGE ! ! !"));
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
