// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "Interfaces/ConquestSelectableInterface.h"
#include "ConquestUnit.generated.h"

UCLASS()
class CONQUEST_API AConquestUnit : public ACharacter, public IConquestSelectableInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AConquestUnit();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	UPROPERTY(BlueprintReadWrite, Category = "AI")
	FVector TargetLocation;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UPROPERTY(BlueprintReadWrite, Replicated, Category = "Team")
	FName TeamName;
	
private:
	virtual bool OnSelectionGained_Implementation() override;

	virtual bool OnSelectionChanged_Implementation(AActor* NewSelection) override;
};
