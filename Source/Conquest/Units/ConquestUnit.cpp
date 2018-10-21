// Fill out your copyright notice in the Description page of Project Settings.

#include "ConquestUnit.h"
#include "Conquest.h"
#include "CapturePoints/CapturePoint.h"


// Sets default values
AConquestUnit::AConquestUnit()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

}

// Called when the game starts or when spawned
void AConquestUnit::BeginPlay()
{
	Super::BeginPlay();
	
}

// Called every frame
void AConquestUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void AConquestUnit::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

}

bool AConquestUnit::OnSelectionGained_Implementation()
{
	UE_LOG(LogConquest, Log, TEXT("I am Selected! (%s)"), *GetNameSafe(this));
	return true;
}

bool AConquestUnit::OnSelectionChanged_Implementation(AActor* NewSelection)
{
	if (NewSelection->GetClass()->IsChildOf(ACapturePoint::StaticClass()))
	{
		UE_LOG(LogConquest, Log, TEXT("New Target Location!"));
		TargetLocation = NewSelection->GetActorLocation();
	}
	return true;
}

