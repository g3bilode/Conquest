// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "ConquestPlayerState.h"
#include "../Capital/Capital.h"
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
	void AttemptSpawnUnit(TSubclassOf<class AConquestUnit> ActorToSpawn, int32 LaneIndex);
	virtual bool AttemptSpawnUnit_Validate(TSubclassOf<class AConquestUnit> ActorToSpawn, int32 LaneIndex);
	virtual void AttemptSpawnUnit_Implementation(TSubclassOf<class AConquestUnit> ActorToSpawn, int32 LaneIndex);

	AConquestPlayerState* GetConquestPlayerState();

	/* Capital */
	void DisplayCapitalMenu(const FVector& Location);
	void SetCapitalMenuVisibility(const bool isVisible) const;

	/* Getters */
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetLaneDestinations(int32 Index);
	class ABarracks* GetBarracksForLane(int32 LaneIndex);

	
	// Reference UMG widget in Editor
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Widgets")
	TSubclassOf<class UUserWidget> wCapitalMenu;


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
	UPROPERTY()
	TArray<class ABarracks*> BarracksArray;


	/* Check if can purchase unit. */
	bool CanPurchaseUnit(const TSubclassOf<class AConquestUnit> unit) const;
	/* Pay the purchase cost of a unit. */
	void PurchaseUnit(const TSubclassOf<class AConquestUnit> unit);
	/* Gather CapturePoints and build lane array. */
	void BuildLaneArray();
	/* Gather Barracks for this team and build array. */
	void BuildBarracksArray();

	UPROPERTY()
	UUserWidget* _menuCapital;

	/* Create UI */
	void CreateUI();
};


