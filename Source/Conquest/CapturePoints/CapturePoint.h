// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interfaces/ConquestSelectableInterface.h"
#include "CapturePoint.generated.h"


UCLASS()
class CONQUEST_API ACapturePoint : public AActor, public IConquestSelectableInterface
{
	GENERATED_BODY()

public:
	ACapturePoint();

	void Capture(int32 TeamIndex);

	int8 GetLaneNumber() const;
	int8 GetRowNumber() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

private:
	virtual bool OnSelectionGained_Implementation(AConquestPlayerController* initiator) override;

	virtual bool OnSelectionChanged_Implementation(AConquestPlayerController* initiator, AActor* NewSelection) override;

	UFUNCTION(BlueprintCallable)
	void OnTriggerOverlapWithUnit(int32 TeamIndex);

	
	/* Team index currently occupying point */
	int32  OccupierTeamIndex;
	/* Lane position (ex: 0 = lane 0) */
	UPROPERTY(EditAnywhere)
	int8 LaneNumber;
	/* Row position (ex: 0 = row 0) */
	UPROPERTY(EditAnywhere)
	int8 RowNumber;
	
};
