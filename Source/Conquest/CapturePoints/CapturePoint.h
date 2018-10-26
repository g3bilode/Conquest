// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interfaces/ConquestSelectableInterface.h"
#include "CapturePoint.generated.h"

UCLASS()
class CONQUEST_API ACapturePoint : public AActor, public IConquestSelectableInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACapturePoint();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	virtual bool OnSelectionGained_Implementation() override;

	virtual bool OnSelectionChanged_Implementation(AConquestPlayerController* initiator, AActor* NewSelection) override;
	
};
