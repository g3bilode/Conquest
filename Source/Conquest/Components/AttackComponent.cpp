// Fill out your copyright notice in the Description page of Project Settings.


#include "AttackComponent.h"
#include "../Conquest.h"
#include "GameFramework/Character.h"

// Sets default values for this component's properties
UAttackComponent::UAttackComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	BaseDamage = 20.0f;
	AttackCooldown = 1.0f;
	AttackRange = 180.0f;
	SetIsReplicatedByDefault(true);
}


void UAttackComponent::DealDamage()
{
	if (IsValid(_TargetActor))
	{
		_TargetActor->TakeDamage(BaseDamage, FDamageEvent(), nullptr, GetOwningActor());
	}
	GetWorld()->GetTimerManager().SetTimer(AttackCooldownTimerHandle, this, &UAttackComponent::OnAttackCooldownExpired, AttackCooldown, false);
}


bool UAttackComponent::IsTargetInRange(AActor* TargetActor)
{
	FVector targetLocation = TargetActor->GetActorLocation();
	return FVector::Dist(targetLocation, GetOwningActor()->GetActorLocation()) <= AttackRange;
}

void UAttackComponent::AttemptAttack(AActor* InTargetActor)
{
	if (!bIsOnCooldown)
	{
		bIsOnCooldown = true;
		_TargetActor = InTargetActor;

		if (IsValid(AttackMontage))
		{
			// Anim montage must call DealDamage
			PlayAttackAnim();
		}
		else
		{
			// Deal damage directly
			DealDamage();
		}
	}
}


void UAttackComponent::PlayAttackAnim_Implementation()
{
	ACharacter* owningCharacter = Cast<ACharacter>(GetOwningActor());
	if (IsValid(owningCharacter))
	{
		owningCharacter->PlayAnimMontage(AttackMontage);
	}
}


void UAttackComponent::OnAttackCooldownExpired()
{
	bIsOnCooldown = false;
}

AActor* UAttackComponent::GetOwningActor()
{
	if (!IsValid(_OwningActor))
	{
		_OwningActor = (AActor*) GetOwner();
	}
	return _OwningActor;
}
