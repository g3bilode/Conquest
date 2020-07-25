// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ConquestPlayerState.h"
#include "Outpost/Outpost.h"
#include "ConquestPlayerController.generated.h"


USTRUCT()
struct FLaneDestinations
{
	GENERATED_BODY()

	FLaneDestinations()
		: LaneDestinations()
	{}

	FLaneDestinations(TArray<FVector> InLaneDestinations)
		: LaneDestinations(InLaneDestinations)
	{}

	UPROPERTY()
	TArray<FVector> LaneDestinations;
};


UCLASS()
class AConquestPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	AConquestPlayerController();

	UFUNCTION(BlueprintCallable, Server, Unreliable, WithValidation)
	void AttemptSpawnUnit(TSubclassOf<class AConquestUnit> ActorToSpawn, const TArray<FVector>& LaneDestinations);
	virtual bool AttemptSpawnUnit_Validate(TSubclassOf<class AConquestUnit> ActorToSpawn, const TArray<FVector>& LaneDestinations);
	virtual void AttemptSpawnUnit_Implementation(TSubclassOf<class AConquestUnit> ActorToSpawn, const TArray<FVector>& LaneDestinations);

	AConquestPlayerState* GetConquestPlayerState();

	/* Outpost */
	void OnOutpostSelect(AOutpost* outpost);
	void DisplayOutpostMenu(const FVector& outpostLocation);
	void SetOutpostMenuVisibility(const bool isVisible) const;
	void AttackOutpost(AOutpost* outpost);

	/* Getters */
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetLaneDestinations(int32 Index) const;

	
	// Reference UMG widget in Editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> wOutpostMenu;


	void OnRep_PlayerState() override;

protected:
	// Begin PlayerController interface
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
	UPROPERTY(Replicated)
	AConquestPlayerState* ConquestPlayerState;
	
	UPROPERTY()
	TArray<FLaneDestinations> LaneArray;


	/* Check if can purchase unit. */
	bool CanPurchaseUnit(const TSubclassOf<class AConquestUnit> unit) const;
	/* Pay the purchase cost of a unit. */
	void PurchaseUnit(const TSubclassOf<class AConquestUnit> unit);
	/* Gather CapturePoints and build lane array. */
	void BuildLaneArray();

	UPROPERTY()
	UUserWidget* _menuOutpost;

	UPROPERTY()
	AOutpost* _selectedOutpost;

	/* Create UI */
	void CreateUI();
};


