// Fill out your copyright notice in the Description page of Project Settings.

#include "Capital.h"
#include "../Conquest.h"
#include "../Components/AttackComponent.h"
#include "../Components/TargetingComponent.h"
#include "../PlayerCharacter/ConquestPlayerController.h"
#include "../PlayerCharacter/ConquestPlayerState.h"
#include "../Units/ConquestUnit.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"

// Sets default values
ACapital::ACapital()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	MaxHealth = 100.f;
	CurrentHealth = MaxHealth;

	// Setup a RootComponent
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>("RootScene");
	RootSceneComponent->SetMobility(EComponentMobility::Static);
	RootComponent = RootSceneComponent;
	// Setup AttackCompontent
	AttackComponent = CreateDefaultSubobject<UAttackComponent>("AttackComponent");
	// Setup TargetingComponent
	TargetingComponent = CreateDefaultSubobject<UTargetingComponent>("TargetingComponent");
}

void ACapital::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		if (TargetingComponent->AcquireTargetEnemy())
		{
			AActor* TargetEnemy = TargetingComponent->GetTargetEnemy();
			if (AttackComponent->IsTargetInRange(TargetEnemy))
			{
				// Attack
				AttackComponent->AttemptAttack(TargetingComponent->GetTargetEnemy());
				// TODO: remove this placeholder
				const FVector& enemyLocation = TargetEnemy->GetActorLocation();
				DrawDebugLine(GetWorld(), GetActorLocation(), enemyLocation, FColor::Magenta);
			}
		}
	}
}

bool ACapital::IsTargetEnemy_Implementation(AActor* OtherActor)
{
	AConquestUnit* otherConquestUnit = Cast<AConquestUnit>(OtherActor);
	if (IsValid(otherConquestUnit))
	{
		return TeamIndex != otherConquestUnit->TeamIndex;
	}
	return false;
}
