// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ConquestPlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Conquest.h"
#include "ConquestCharacter.h"
#include "Engine/LocalPlayer.h"
#include "CapturePoints/CapturePoint.h"
#include "Units/ConquestUnit.h"
#include "Kismet/GameplayStatics.h"
#include "SpawnPoints/SpawnPoint.h"

AConquestPlayerController::AConquestPlayerController()
{
	bShowMouseCursor = true;
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

bool AConquestPlayerController::AttemptSpawnUnit_Validate(TSubclassOf<class AConquestUnit> actorToSpawn)
{
	return true;
}

void AConquestPlayerController::AttemptSpawnUnit_Implementation(TSubclassOf<class AConquestUnit> actorToSpawn)
{
	
	if (!AttemptPurchaseUnit(actorToSpawn))
	{
		UE_LOG(LogConquest, Log, TEXT("Cannot purchase unit."));
		return;
	}

	FVector location;
	TArray<AActor*> unitSpawnActors;
	UGameplayStatics::GetAllActorsOfClass(GetWorld(), ASpawnPoint::StaticClass(), unitSpawnActors);
	for (AActor* unitSpawnActor : unitSpawnActors)
	{
		ASpawnPoint* spawnPoint = Cast<ASpawnPoint>(unitSpawnActor);
		if (spawnPoint != nullptr && ConquestPlayerState != nullptr)
		{
			if (spawnPoint->TeamName == ConquestPlayerState->TeamName)
			{
				location = spawnPoint->GetActorLocation();
				location += FVector(0, 0, 200);
				break;
			}

		}
	}

	FTransform spawnTransform;
	spawnTransform.SetLocation(location);

	AActor* spawnedUnit = UGameplayStatics::BeginDeferredActorSpawnFromClass(this, actorToSpawn->GetDefaultObject()->GetClass(), spawnTransform);
	
	AConquestUnit* conquestSpawnedUnit = Cast<AConquestUnit>(spawnedUnit);

	if (ConquestPlayerState != nullptr && IsValid(conquestSpawnedUnit))
	{
		conquestSpawnedUnit->TeamName = ConquestPlayerState->TeamName;

		UGameplayStatics::FinishSpawningActor(spawnedUnit, spawnTransform);
	}
}

void AConquestPlayerController::PlayerTick(float DeltaTime)
{
	Super::PlayerTick(DeltaTime);
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
	APawn* const Pawn = GetPawn();
	AConquestCharacter* character = Cast<AConquestCharacter>(Pawn);
	if (character)
	{
		character->MoveCharacterForward(axisValue);
	}
}

void AConquestPlayerController::OnMoveRightAxis(float axisValue)
{
	APawn* const Pawn = GetPawn();
	AConquestCharacter* character = Cast<AConquestCharacter>(Pawn);
	if (character)
	{
		character->MoveCharacterRight(axisValue);
	}
}

void AConquestPlayerController::OnMouseHorizontal(float axisValue)
{
	if (lookAroundEnabled)
	{
		APawn* const Pawn = GetPawn();
		Pawn->AddControllerYawInput(axisValue);
		Cast<ULocalPlayer>(Player)->ViewportClient->Viewport->SetMouse(mouseLockPositionX, mouseLockPositionY);
	}
}

void AConquestPlayerController::OnMouseVertical(float axisValue)
{
	if (lookAroundEnabled)
	{
		APawn* const Pawn = GetPawn();
		AConquestCharacter* character = Cast<AConquestCharacter>(Pawn);
		if (character)
		{
			character->RotateCameraArm(FRotator(axisValue, 0.0f, 0.0f));
		}
		Cast<ULocalPlayer>(Player)->ViewportClient->Viewport->SetMouse(mouseLockPositionX, mouseLockPositionY);
	}
}

void AConquestPlayerController::OnZoomInAction()
{
	APawn* const Pawn = GetPawn();
	AConquestCharacter* character = Cast<AConquestCharacter>(Pawn);
	if (character)
	{
		character->ChangeCameraArmLength(-1.0f);
	}
}

void AConquestPlayerController::OnZoomOutAction()
{
	APawn* const Pawn = GetPawn();
	AConquestCharacter* character = Cast<AConquestCharacter>(Pawn);
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
		APawn* const Pawn = GetPawn();
		AConquestCharacter* const character = Cast<AConquestCharacter>(Pawn);
		if (character)
		{
			character->OnSelectActor(SelectedActor.Get());
		}
	}
}

bool AConquestPlayerController::AttemptPurchaseUnit(const TSubclassOf<class AConquestUnit> unit)
{
	int32 cost = unit->GetDefaultObject<AConquestUnit>()->PurchaseCost;
	if (cost <= ConquestPlayerState->Gold)
	{
		ConquestPlayerState->Gold -= cost;
		return true;
	}
	return false;
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

void AConquestPlayerController::MoveUnit_Implementation(AConquestUnit* unit, FVector location)
{
	unit->SetTargetDestination(location);
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


void AConquestPlayerController::SetOutpostMenuLocation(const FVector2D screenLocation) const
{
	_menuOutpost->SetPositionInViewport(screenLocation);
}

bool AConquestPlayerController::MoveUnit_Validate(AConquestUnit* unit, FVector location)
{
	return true;
}
