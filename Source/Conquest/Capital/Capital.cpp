// Fill out your copyright notice in the Description page of Project Settings.

#include "Capital.h"
#include "../Conquest.h"
#include "../Components/AttackComponent.h"
#include "../Components/HealthComponent.h"
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
	SetReplicates(true);

	// Setup a RootComponent
	RootSceneComponent = CreateDefaultSubobject<USceneComponent>("RootScene");
	RootSceneComponent->SetMobility(EComponentMobility::Static);
	RootComponent = RootSceneComponent;
	// Setup AttackCompontent
	AttackComponent = CreateDefaultSubobject<UAttackComponent>("AttackComponent");
	// Setup TargetingComponent
	TargetingComponent = CreateDefaultSubobject<UTargetingComponent>("TargetingComponent");
	// Setup HealthComponent
	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
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


float ACapital::TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser)
{
	// TODO: Only server should need to do this
	float ActualDamage = Super::TakeDamage(DamageAmount, DamageEvent, EventInstigator, DamageCauser);
	HealthComponent->TakeDamage(ActualDamage);
	if (HealthComponent->IsDead())
	{
		UE_LOG(LogConquest, Log, TEXT("CAPITAL LOST: %s"), *GetNameSafe(this));
	}
	return ActualDamage;
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
