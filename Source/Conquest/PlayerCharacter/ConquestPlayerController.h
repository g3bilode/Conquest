// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ConquestPlayerState.h"
#include "ConquestPlayerController.generated.h"


UCLASS()
class AConquestPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AConquestPlayerController();

	UFUNCTION(BlueprintCallable, Server, Unreliable, WithValidation)
	void AttemptSpawnUnit(TSubclassOf<class AConquestUnit> actorToSpawn);
	virtual bool AttemptSpawnUnit_Validate(TSubclassOf<class AConquestUnit> actorToSpawn);
	virtual void AttemptSpawnUnit_Implementation(TSubclassOf<class AConquestUnit> actorToSpawn);

	UFUNCTION(Server, Unreliable, WithValidation)
	void MoveUnit(AConquestUnit* unit, FVector location);
	virtual bool MoveUnit_Validate(AConquestUnit* unit, FVector location);
	virtual void MoveUnit_Implementation(AConquestUnit* unit, FVector location);
	

protected:
	// Begin PlayerController interface
	virtual void PlayerTick(float DeltaTime) override;
	virtual void SetupInputComponent() override;
	virtual void BeginPlay() override;
	// End PlayerController interface

	// Input handlers
	void OnMoveForwardAxis(float axisValue);
	void OnMoveRightAxis(float axisValue);
	void OnMouseHorizontal(float axisValue);
	void OnMouseVertical(float axisValue);
	void OnZoomInAction();
	void OnZoomOutAction();
	void OnLookAroundStart();
	void OnLookAroundStop();
	void OnSelect();

private:
	bool lookAroundEnabled = false;
	int32 mouseLockPositionX;
	int32 mouseLockPositionY;

	TWeakObjectPtr<AActor> SelectedActor;
	AConquestPlayerState* ConquestPlayerState;

	/* Attempt to purchase unit. Return True on success. */
	bool AttemptPurchaseUnit(const TSubclassOf<class AConquestUnit> unit);
};


