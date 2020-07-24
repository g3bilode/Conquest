// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/ConquestSelectableInterface.h"
#include "CapturePoint.generated.h"


USTRUCT()
struct FUnitSlot
{
	GENERATED_BODY()
		
	FUnitSlot()
		: SlotLocation(ForceInitToZero)
		, IsOccupied(false)
		, TeamName(NAME_None)
	{}
	FUnitSlot(FVector inSlotLocation, FName inTeamName)
		: SlotLocation(inSlotLocation)
		, IsOccupied(false)
		, TeamName(inTeamName)
	{}

	UPROPERTY()
	FVector SlotLocation;
	UPROPERTY()
	bool IsOccupied;
	UPROPERTY()
	FName TeamName;

};


UCLASS()
class CONQUEST_API ACapturePoint : public AActor, public IConquestSelectableInterface
{
	GENERATED_BODY()

public:
	ACapturePoint();

	FVector* GetDestinationForUnit(const FName teamName);

	void Capture(FName TeamName);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	virtual bool OnSelectionGained_Implementation(AConquestPlayerController* initiator) override;

	virtual bool OnSelectionChanged_Implementation(AConquestPlayerController* initiator, AActor* NewSelection) override;

	void GenerateUnitSlots();
	void GenerateUnitSlotsForTeam(const FName teamName, const int32 teamIndex);
	FUnitSlot* GetAvailableUnitSlot(const FName teamName);

	UFUNCTION()
	virtual void OnRep_UnitSlots();

	UFUNCTION(BlueprintCallable)
	void OnTriggerOverlapWithUnit(FName TeamName);

	// UnitSlot configs
	int32 UnitSlotBuffer;
	int32 UnitSlotCount;
	int32 UnitSlotColumnCount;
	
	// Temp locations for units to occupy
	UPROPERTY(ReplicatedUsing=OnRep_UnitSlots)
	TArray<FUnitSlot> UnitSlots;
	
	// Flag to draw debug slots only once
	bool AreSlotsDrawn;
	
	// TeamName currently occupying point
	FName OccupierTeamName;

	// Two digit representation of position (ex: 21 = lane 2, row 1)
	UPROPERTY(EditAnywhere)
	FName LanePosition;
	
};
