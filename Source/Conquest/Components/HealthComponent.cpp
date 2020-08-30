// Fill out your copyright notice in the Description page of Project Settings.


#include "HealthComponent.h"
#include "Net/UnrealNetwork.h"


// Sets default values for this component's properties
UHealthComponent::UHealthComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

	BaseHealth = 100.0f;
	CurrentHealth = GetMaxHealth();

	bIsDead = false;

	SetIsReplicatedByDefault(true);
}


void UHealthComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(UHealthComponent, CurrentHealth);
	DOREPLIFETIME(UHealthComponent, bIsDead);
}


float UHealthComponent::GetMaxHealth() const
{
	return BaseHealth;
}


float UHealthComponent::GetCurrentHealth() const
{
	return CurrentHealth;
}


void UHealthComponent::TakeDamage(float Damage)
{
	CurrentHealth -= Damage;
	if (CurrentHealth <= 0.0f)
	{
		bIsDead = true;
	}
}


bool UHealthComponent::IsDead() const
{
	return bIsDead;
}
