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

	/* Check if can make purchase. */
	bool CanMakePurchase(int32 Cost) const;

	/* Make a purchase on server. */
	UFUNCTION(Server, Reliable, WithValidation)
	void MakePurchase(int32 Cost);
	virtual bool MakePurchase_Validate(int32 Cost);
	virtual void MakePurchase_Implementation(int32 Cost);

	UFUNCTION(Server, Reliable, WithValidation)
	void SpawnUnit(TSubclassOf<class AConquestUnit> UnitToSpawn, FVector Location, int32 TeamIndex, int32 LaneIndex, const TArray<FVector>& LaneDestinations);
	virtual bool SpawnUnit_Validate(TSubclassOf<class AConquestUnit> UnitToSpawn, FVector Location, int32 TeamIndex, int32 LaneIndex, const TArray<FVector>& LaneDestinations);
	virtual void SpawnUnit_Implementation(TSubclassOf<class AConquestUnit> UnitToSpawn, FVector Location, int32 TeamIndex, int32 LaneIndex, const TArray<FVector>& LaneDestinations);

	AConquestPlayerState* GetConquestPlayerState();

	/* Getters */
	UFUNCTION(BlueprintCallable)
	TArray<FVector> GetLaneDestinations(int32 Index);
	/* Get Friendly Capital Health Percent */
	UFUNCTION(BlueprintPure)
	float GetFriendlyCapitalHealthPercent();

	/* Enable spawning mode. */
	void EnableSpawningMode(TSubclassOf<class AUnitSpawner> SpawnerClass);


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
	void OnClick(bool WithShift);

private:
	bool lookAroundEnabled = false;
	int32 mouseLockPositionX;
	int32 mouseLockPositionY;

	/* Is in spawning mode? */
	bool bIsSpawningMode;
	/* Currently active spawner object. */
	class AUnitSpawner* ActiveSpawner;

	TWeakObjectPtr<AActor> SelectedActor;
	UPROPERTY(ReplicatedUsing=OnRep_ConquestPlayerState)
	AConquestPlayerState* ConquestPlayerState;
	
	UPROPERTY()
	TArray<FLaneDestinations> LaneArray;
	UPROPERTY()
	TArray<class ABarracks*> BarracksArray;

	/* Friendly Capital */
	UPROPERTY()
	class ACapital* FriendlyCapital;
	/* Enemy Capital. */
	UPROPERTY()
	class ACapital* EnemyCapital;

	/* On rep player state */
	UFUNCTION()
	void OnRep_ConquestPlayerState();

	/* Gather CapturePoints and build lane array. */
	void BuildLaneArray();
	/* Gather Barracks for this team and build array. */
	void BuildBarracksArray();
	/* Gather Capitals. */
	void GatherCapitals();
	/* Update active unit spawner location. */
	void UpdateSpawnerPosition();
};


