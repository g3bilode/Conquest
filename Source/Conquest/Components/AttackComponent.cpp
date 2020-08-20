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
	SetIsReplicatedByDefault(true);
}



void UAttackComponent::DealDamage(AActor* TargetActor)
{
	if (IsValid(TargetActor))
	{
		ACharacter* owningCharacter = (ACharacter*)GetOwner();
		TargetActor->TakeDamage(BaseDamage, FDamageEvent(), owningCharacter->GetController(), owningCharacter);
	}
	GetWorld()->GetTimerManager().SetTimer(AttackCooldownTimerHandle, this, &UAttackComponent::OnAttackCooldownExpired, AttackCooldown, false);
}


void UAttackComponent::AttemptAttack()
{
	if (!bIsOnCooldown)
	{
		bIsOnCooldown = true;
		PlayAttackAnim();
	}
}


void UAttackComponent::PlayAttackAnim_Implementation()
{
	// TODO: Save owner?
	ACharacter* owningCharacter = (ACharacter*)GetOwner();
	owningCharacter->PlayAnimMontage(AttackMontage);
}


void UAttackComponent::OnAttackCooldownExpired()
{
	bIsOnCooldown = false;
}
