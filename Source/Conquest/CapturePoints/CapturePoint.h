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
		: slotLocation(ForceInitToZero)
		, isOccupied(false)
	{}
	FUnitSlot(FVector inSlotLocation)
		: slotLocation(inSlotLocation)
		, isOccupied(false)
	{}

	FVector slotLocation;
	bool isOccupied;

};


UCLASS()
class CONQUEST_API ACapturePoint : public AActor, public IConquestSelectableInterface
{
	GENERATED_BODY()

public:
	ACapturePoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	virtual bool OnSelectionGained_Implementation(AConquestPlayerController* initiator) override;

	virtual bool OnSelectionChanged_Implementation(AConquestPlayerController* initiator, AActor* NewSelection) override;

	void generateUnitSlots();
	FUnitSlot* getAvailableUnitSlot();

	int32 unitSlotBuffer;
	int32 unitSlotCount;
	int32 unitSlotColumnCount;

	TArray<FUnitSlot*> unitSlots;
	
};
