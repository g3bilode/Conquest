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

	FVector SlotLocation;
	bool IsOccupied;
	FName TeamName;

};


UCLASS()
class CONQUEST_API ACapturePoint : public AActor, public IConquestSelectableInterface
{
	GENERATED_BODY()

public:
	ACapturePoint();

	FVector* getDestinationForUnit(const FName teamName);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	virtual bool OnSelectionGained_Implementation(AConquestPlayerController* initiator) override;

	virtual bool OnSelectionChanged_Implementation(AConquestPlayerController* initiator, AActor* NewSelection) override;

	void generateUnitSlots();
	void generateUnitSlotsForTeam(const FName teamName, const int32 teamIndex);
	FUnitSlot* getAvailableUnitSlot(const FName teamName);

	int32 UnitSlotBuffer;
	int32 UnitSlotCount;
	int32 UnitSlotColumnCount;
	
	UPROPERTY()
	TArray<FUnitSlot> UnitSlots;
	
};
