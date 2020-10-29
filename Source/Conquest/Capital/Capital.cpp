// Fill out your copyright notice in the Description page of Project Settings.

#include "Capital.h"
#include "../Conquest.h"
#include "../Abilities/ConquestAbilitySystemComponent.h"
#include "../Abilities/ConquestAttributeSet.h"
#include "../Abilities/ConquestGameplayAbility.h"
#include "../Components/AttackComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/TargetingComponent.h"
#include "../PlayerCharacter/ConquestPlayerController.h"
#include "../PlayerCharacter/ConquestPlayerState.h"
#include "../Units/ConquestUnit.h"
#include "AbilitySystemComponent.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffectTypes.h"
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

	// Setup Ability System Component
	AbilitySystemComponent = CreateDefaultSubobject<UConquestAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	// Setup Attributes
	Attributes = CreateDefaultSubobject<UConquestAttributeSet>("Attributes");
}


UAbilitySystemComponent* ACapital::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


void ACapital::BeginPlay()
{
	Super::BeginPlay();
	
	// Initialize Ability System
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	InitializeAttributes();
	if (HasAuthority())
	{
		GiveAbilities_Auth();
	}

	// Bind attribute change events
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetHealthAttribute()).AddUObject(this, &ACapital::OnHealthChanged);
}


void ACapital::InitializeAttributes()
{
	if (IsValid(AbilitySystemComponent) && IsValid(DefaultAttributeEffect))
	{
		FGameplayEffectContextHandle EffectContext = AbilitySystemComponent->MakeEffectContext();
		EffectContext.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(DefaultAttributeEffect, 1, EffectContext);
		if (SpecHandle.IsValid())
		{
			FActiveGameplayEffectHandle GEHandle = AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}


void ACapital::GiveAbilities_Auth()
{
	if (HasAuthority() && IsValid(AbilitySystemComponent))
	{
		for (TSubclassOf<UConquestGameplayAbility>& DefaultAbility : AttackComponent->DefaultAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DefaultAbility, 1, INDEX_NONE, this));
		}
	}
}


void ACapital::OnHealthChanged(const struct FOnAttributeChangeData& Data)
{
	float damage = Data.OldValue - Data.NewValue;
	TakeDamage(damage, FDamageEvent(), nullptr, this); //TODO fix this
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
