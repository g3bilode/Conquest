// Fill out your copyright notice in the Description page of Project Settings.

#include "ConquestUnit.h"
#include "../Capital/Capital.h"
#include "../Components/AttackComponent.h"
#include "../Components/HealthComponent.h"
#include "../Components/TargetingComponent.h"
#include "../Components/UnitMovementComponent.h"
#include "../GameState/ConquestGameState.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Net/UnrealNetwork.h"


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
}


float AConquestUnit::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// TODO: Only server should need to do this
	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	HealthComponent->TakeDamage(ActualDamage);
	if (HealthComponent->IsDead())
	{
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
			AActor* TargetEnemy = TargetingComponent->GetTargetEnemy();
			if (AttackComponent->IsTargetInRange(TargetEnemy))
			{
				// Attack
				FaceTargetEnemy(DeltaTime);
				AttackComponent->AttemptAttack(TargetEnemy);
			}
			else
			{
				MovementComponent->MoveToEnemy(TargetEnemy->GetActorLocation());
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

void AConquestUnit::OnAttackAnimHit()
{
	if (HasAuthority())
	{
		AttackComponent->DealDamage();
	}
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


bool AConquestUnit::IsEnemyInMyLane(AConquestUnit* ConquestUnit)
{
	// TODO: Improve this?
	// Difference in lane index between teams (ie: 0 = 2)
	const int32 laneDiff = 2;
	return (LaneIndex + ConquestUnit->LaneIndex) == laneDiff;
}
