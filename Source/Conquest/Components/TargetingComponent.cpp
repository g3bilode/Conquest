// Fill out your copyright notice in the Description page of Project Settings.


#include "TargetingComponent.h"
#include "../Interfaces/TargeterInterface.h"
#include "../Units/ConquestUnit.h"
#include "Components/SphereComponent.h"


DEFINE_LOG_CATEGORY_STATIC(LogConquestTargeting, Log, All);


// Sets default values for this component's properties
UTargetingComponent::UTargetingComponent()
{
	// Setup Collision Spheres
	AggroSphere = CreateDefaultSubobject<USphereComponent>("AggroSphere");
	AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &UTargetingComponent::OnAggroCollision);

	AggroSphereRadius = 300.0f;
}


void UTargetingComponent::BeginPlay()
{
	Super::BeginPlay();

	AActor* owningActor = GetOwner();
	AggroSphere->SetSphereRadius(AggroSphereRadius);
	AggroSphere->AttachToComponent(owningActor->GetRootComponent(), FAttachmentTransformRules::KeepRelativeTransform);
}


AActor* UTargetingComponent::GetTargetEnemy() const
{
	return TargetEnemy;
}


bool UTargetingComponent::AcquireTargetEnemy()
{
	if (!IsValidTarget(TargetEnemy))
	{
		// Find new enemy
		if (KnownEnemies.Num() > 0)
		{
			for (int32 i = KnownEnemies.Num() - 1; i >= 0; i--)
			{
				AActor* knownEnemy = KnownEnemies[i];
				if (IsValidTarget(knownEnemy))
				{
					// New target found
					TargetEnemy = knownEnemy;
					return true;
				}
				else
				{
					KnownEnemies.RemoveAt(i);
				}
			}
		}
		// Found none
		return false;
	}
	// Already have target
	return true;
}


void UTargetingComponent::OnAggroCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	// TODO: This may only need to be done on server?
	// Find a new target
	if (GetNameSafe(OtherComp) == FString("CollisionCylinder"))
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
	// TODO: cheating, should fix this and remove cyclical dependency
	AConquestUnit* conquestUnit = Cast<AConquestUnit>(TargetActor);
	return IsValid(conquestUnit) && !conquestUnit->IsDead();
}
