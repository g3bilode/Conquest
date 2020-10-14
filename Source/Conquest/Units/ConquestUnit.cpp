// Fill out your copyright notice in the Description page of Project Settings.

#include "ConquestUnit.h"
#include "../Abilities/ConquestAbilitySystemComponent.h"
#include "../Abilities/ConquestAttributeSet.h"
#include "../Abilities/ConquestGameplayAbility.h"
#include "../Capital/Capital.h"
#include "../Components/AttackComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/TargetingComponent.h"
#include "../Components/UnitMovementComponent.h"
#include "../GameState/ConquestGameState.h"
#include "../HUD/ResourceDripComponent.h"
#include "../PlayerCharacter/ConquestPlayerController.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameplayAbilitySpec.h"
#include "GameplayEffectTypes.h"
#include "Net/UnrealNetwork.h"


DEFINE_LOG_CATEGORY_STATIC(LogConquestUnit, Log, All);

// Sets default values
AConquestUnit::AConquestUnit()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setup UnitMovementCompontent
	MovementComponent = CreateDefaultSubobject<UUnitMovementComponent>("MovementComponent");
	// Setup AttackCompontent
	AttackComponent = CreateDefaultSubobject<UAttackComponent>("AttackComponent");
	// Setup TargetingComponent
	TargetingComponent = CreateDefaultSubobject<UTargetingComponent>("TargetingComponent");
	// Setup HealthComponent
	HealthComponent = CreateDefaultSubobject<UHealthComponent>("HealthComponent");
	// Setup ResourceDripComponent
	ResourceDripComponent = CreateDefaultSubobject<UResourceDripComponent>("ResourceDripComponent");
	ResourceDripComponent->AttachToComponent(RootComponent, FAttachmentTransformRules::KeepRelativeTransform);
	
	// Setup Ability System Component
	AbilitySystemComponent = CreateDefaultSubobject<UConquestAbilitySystemComponent>("AbilitySystemComponent");
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);
	// Setup Attributes
	Attributes = CreateDefaultSubobject<UConquestAttributeSet>("Attributes");
}


UAbilitySystemComponent* AConquestUnit::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}


float AConquestUnit::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// TODO: Only server should need to do this
	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	HealthComponent->TakeDamage(ActualDamage);
	if (HealthComponent->IsDead())
	{
		AConquestGameState* conquestGameState = Cast<AConquestGameState>(GetWorld()->GetGameState());
		AConquestUnit* conquestUnit = Cast<AConquestUnit>(DamageCauser);
		if (IsValid(conquestGameState) && IsValid(conquestUnit))
		{
			// Only Units can gain bounty on kill
			conquestGameState->AwardBounty(Bounty, conquestUnit->TeamIndex);
			// Display bounty
			DisplayBountyForTeam(conquestUnit->TeamIndex);
		}
		DeathBegin();
	}
	return ActualDamage;
}


bool AConquestUnit::IsTargetEnemy_Implementation(AActor* OtherActor)
{
	// TODO: create team component
	AConquestUnit* otherConquestUnit = Cast<AConquestUnit>(OtherActor);
	if (IsValid(otherConquestUnit))
	{
		return (otherConquestUnit->TeamIndex != TeamIndex) && IsEnemyInMyLane(otherConquestUnit);
	}
	ACapital* otherCapital = Cast<ACapital>(OtherActor);
	if (IsValid(otherCapital))
	{
		return (otherCapital->TeamIndex != TeamIndex);
	}
	return false;
}


void AConquestUnit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME_CONDITION(AConquestUnit, TeamIndex, COND_InitialOnly);
	DOREPLIFETIME_CONDITION(AConquestUnit, LaneIndex, COND_InitialOnly);
}


// Called when the game starts or when spawned
void AConquestUnit::BeginPlay()
{
	Super::BeginPlay();

	// Setup unit colour based on team
	UMaterialInterface* material = GetMesh()->GetMaterial(0);
	UMaterialInstanceDynamic* dynamicMaterial = GetMesh()->CreateDynamicMaterialInstance(0, material);
	if (TeamIndex == 0)
	{
		dynamicMaterial->SetVectorParameterValue("BodyColor", BaseColour);
	}
	else
	{
		dynamicMaterial->SetVectorParameterValue("BodyColor", AlternateColour);
	}
	GetMesh()->SetMaterial(0, dynamicMaterial);

	// Setup resource drip
	ResourceDripComponent->SetupWidget(FLinearColor(0.863f, 0.573f, 0.043f));


	// Bind combat phase start delegate
	if (HasAuthority())
	{
		AConquestGameState* conquestGameState = Cast<AConquestGameState>(GetWorld()->GetGameState());
		if (IsValid(conquestGameState))
		{
			conquestGameState->CombatPhase_OnStart.AddDynamic(this, &AConquestUnit::RespondToCombatPhaseBegin);
		}
	}

	// Avoidance
	UCharacterMovementComponent* characterMovement = GetCharacterMovement();
	characterMovement->SetAvoidanceGroup(TeamIndex + 1);
	characterMovement->SetGroupsToAvoid(TeamIndex + 1);
	characterMovement->AvoidanceConsiderationRadius = 50.f;
	characterMovement->SetAvoidanceEnabled(true);

	// Initialize Ability System
	AbilitySystemComponent->InitAbilityActorInfo(this, this);
	InitializeAttributes();
	if (HasAuthority())
	{
		GiveAbilities_Auth();
	}

	// Bind attribute change events
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Attributes->GetHealthAttribute()).AddUObject(this, &AConquestUnit::OnHealthChanged);

	// Disable until combat phase
	SetActorTickEnabled(false);
}


// Called every frame
void AConquestUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		if (HealthComponent->IsDead())
		{
			return;
		}

		if (TargetingComponent->AcquireTargetEnemy())
		{
			// Move towards enemy
			AActor* targetEnemy = TargetingComponent->GetTargetEnemy();
			if (AttackComponent->IsTargetInRange(targetEnemy))
			{
				// Attack
				FaceTargetEnemy(DeltaTime);
				AttackComponent->AttemptAttack(targetEnemy);
			}
			else
			{
				MovementComponent->MoveToEnemy(targetEnemy->GetActorLocation());
			}
		}
		else
		{
			// Move towards next point
			MovementComponent->MoveThroughLane();
		}
	}
}


void AConquestUnit::SetLaneDestinations(const TArray<FVector>& InLaneDestinations)
{
	MovementComponent->SetLaneDestinations(InLaneDestinations);
}


void AConquestUnit::DeathEnd()
{
	if (HasAuthority())
	{
		bool bWasDestroyed = Destroy();
		AConquestGameState* conquestGameState = (AConquestGameState*)GetWorld()->GetGameState();
		conquestGameState->OnUnitDeath();
	}
}


void AConquestUnit::InitializeAttributes()
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


void AConquestUnit::GiveAbilities_Auth()
{
	if (HasAuthority() && IsValid(AbilitySystemComponent))
	{
		for (TSubclassOf<UConquestGameplayAbility>& DefaultAbility : AttackComponent->DefaultAbilities)
		{
			AbilitySystemComponent->GiveAbility(FGameplayAbilitySpec(DefaultAbility, 1, INDEX_NONE, this));
		}
	}
}


void AConquestUnit::RespondToCombatPhaseBegin()
{
	SetActorTickEnabled(true);
}


void AConquestUnit::FaceTargetEnemy(float DeltaTime)
{
	// Face enemy
	FVector targetDirection = TargetingComponent->GetTargetEnemy()->GetActorLocation() - GetActorLocation();
	targetDirection.Normalize(SMALL_NUMBER);
	FRotator newRotation = FMath::RInterpTo(GetActorRotation(), targetDirection.Rotation(), DeltaTime, 1.0);
	newRotation.Pitch = 0;
	SetActorRotation(newRotation);
}


void AConquestUnit::DeathBegin()
{
	PlayDeathAnim();
}


void AConquestUnit::PlayDeathAnim_Implementation()
{
	// Ready to die
	PlayAnimMontage(DeathMontage);
}


void AConquestUnit::DisplayBountyForTeam(int32 KillerTeamIndex) const
{
	AConquestGameState* conquestGameState = Cast<AConquestGameState>(GetWorld()->GetGameState());
	if (IsValid(conquestGameState))
	{
		for (AConquestPlayerState* conquestPlayerState : conquestGameState->GetConquestPlayerArray())
		{
			if (conquestPlayerState->TeamIndex == KillerTeamIndex)
			{
				// Display bounty for killing player
				AConquestPlayerController* conquestPlayerController = (AConquestPlayerController*)conquestPlayerState->GetOwner();
				if (IsValid(conquestPlayerController))
				{
					conquestPlayerController->DisplayBounty(this);
				}
				return;
			}
		}
	}
}


void AConquestUnit::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	float damage = Data.OldValue - Data.NewValue;
	TakeDamage(damage, FDamageEvent(), nullptr, this); //TODO fix this
}


void AConquestUnit::DisplayBounty() const
{
	ResourceDripComponent->ActivateDripWidget(Bounty);
}


bool AConquestUnit::IsEnemyInMyLane(AConquestUnit* ConquestUnit)
{
	// TODO: Improve this?
	// Difference in lane index between teams (ie: 0 = 2)
	const int32 laneDiff = 2;
	return (LaneIndex + ConquestUnit->LaneIndex) == laneDiff;
}
