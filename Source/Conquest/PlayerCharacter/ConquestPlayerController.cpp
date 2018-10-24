// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ConquestPlayerController.h"
#include "ConquestCharacter.h"
#include "Engine/LocalPlayer.h"
#include "CapturePoints/CapturePoint.h"
#include "Units/ConquestUnit.h"

AConquestPlayerController::AConquestPlayerController()
{
	bShowMouseCursor = true;
}

bool AConquestPlayerController::AttemptSpawnUnit_Validate(TSubclassOf<class AConquestUnit> actorToSpawn, FVector location)
{
	return true;
}

void AConquestPlayerController::AttemptSpawnUnit_Implementation(TSubclassOf<class AConquestUnit> actorToSpawn, FVector location)
{
	AActor* spawnedUnit = GetWorld()->SpawnActor(actorToSpawn->GetDefaultObject()->GetClass(), &location);
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
				IConquestSelectableInterface::Execute_OnSelectionChanged(SelectedActor.Get(), NewSelection);
			}

			// Selection action
			if (NewSelection->GetClass()->ImplementsInterface(UConquestSelectableInterface::StaticClass()))
			{
				IConquestSelectableInterface::Execute_OnSelectionGained(NewSelection);
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
