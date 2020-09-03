// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetingComponent.h"
#include "../Interfaces/TargeterInterface.h"
#include "Components/SphereComponent.h"
#include "HealthComponent.h"


DEFINE_LOG_CATEGORY_STATIC(LogConquestTargeting, Log, All);


// Sets default values for this component's properties
UTargetingComponent::UTargetingComponent()
{
	// Setup Collision Spheres
	AggroSphere = CreateDefaultSubobject<USphereComponent>("AggroSphere");
	AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &UTargetingComponent::OnAggroCollision);

	AggroSphereRadius = 300.0f;
}


void UTargetingComponent::OnRegister()
{
	Super::OnRegister();
	AggroSphere->SetSphereRadius(AggroSphereRadius);
	AggroSphere->AttachToComponent(GetOwner()->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
	_OwningActor = GetOwner();
}


AActor* UTargetingComponent::GetTargetEnemy() const
{
	return TargetEnemy;
}


bool UTargetingComponent::AcquireTargetEnemy()
{
	if (!IsValidTarget(TargetEnemy))
	{
		TargetEnemy = nullptr;
		// Find new enemy
		if (KnownEnemies.Num() > 0)
		{
			float nearestEnemyDistance = MAX_FLT;
			FVector currentLocation = _OwningActor->GetActorLocation();
			for (int32 i = KnownEnemies.Num() - 1; i >= 0; i--)
			{
				AActor* knownEnemy = KnownEnemies[i];
				if (IsValidTarget(knownEnemy))
				{
					float enemyDistance = FVector::Dist(currentLocation, knownEnemy->GetActorLocation());
					if (enemyDistance < nearestEnemyDistance)
					{
						// New target found
						TargetEnemy = knownEnemy;
						nearestEnemyDistance = enemyDistance;
						continue;
					}
				}
				else
				{
					KnownEnemies.RemoveAt(i);
				}
			}
		}
		// Valid if we've found a new target
		return IsValid(TargetEnemy);
	}
	// Already have target
	return true;
}


void UTargetingComponent::OnAggroCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	// TODO: This may only need to be done on server?
	// Objects with a "HitSphere" have a HealthComponent, so assume valid target
	if (GetNameSafe(OtherComp) == FString("HitSphere"))
	{
		if (ensure(GetOwner()->GetClass()->ImplementsInterface(UTargeterInterface::StaticClass())))
		{
			bool bIsTargetEnemy = ITargeterInterface::Execute_IsTargetEnemy(GetOwner(), OtherActor);
			if (bIsTargetEnemy)
			{
				KnownEnemies.Add(OtherActor);
			}
		}
	}
}


bool UTargetingComponent::IsValidTarget(AActor* TargetActor)
{
	if (IsValid(TargetActor))
	{
		UHealthComponent* targetHealthComponent = (UHealthComponent*)TargetActor->GetComponentByClass(UHealthComponent::StaticClass());
		return IsValid(targetHealthComponent) && !targetHealthComponent->IsDead();
	}
	return false;
}
