// Fill out your copyright notice in the Description page of Project Settings.

#include "ConquestUnit.h"
#include "../Conquest.h"
#include "../Components/AttackComponent.h"
#include "../Components/TargetingComponent.h"
#include "../Components/UnitMovementComponent.h"
#include "../GameState/ConquestGameState.h"
#include "Net/UnrealNetwork.h"


const FLinearColor AConquestUnit::AlternateColour(0.29f, 0.57f, 0.79f);

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

	AttackRange = 180.0f;
	Health = 100.0f;

	bIsDead = false;
}


float AConquestUnit::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	// TODO: Only server should need to do this
	float ActualDamage = Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
	Health -= ActualDamage;
	if (Health <= 0.0f)
	{
		DeathBegin();
	}
	return ActualDamage;
}

bool AConquestUnit::IsTargetEnemy_Implementation(AActor* OtherActor)
{
	AConquestUnit* otherConquestUnit = Cast<AConquestUnit>(OtherActor);
	if (IsValid(otherConquestUnit))
	{
		return (otherConquestUnit->TeamIndex != TeamIndex) && IsEnemyInMyLane(otherConquestUnit);
	}
	return false;
}

void AConquestUnit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AConquestUnit, TeamName);
	DOREPLIFETIME(AConquestUnit, TeamIndex);
	DOREPLIFETIME(AConquestUnit, LaneIndex);
	DOREPLIFETIME(AConquestUnit, Health);
	DOREPLIFETIME(AConquestUnit, bIsDead);
}

// Called when the game starts or when spawned
void AConquestUnit::BeginPlay()
{
	Super::BeginPlay();

	if (TeamIndex > 0)
	{
		// Use alternate colour
		UMaterialInterface* material = GetMesh()->GetMaterial(0);
		UMaterialInstanceDynamic* dynamicMaterial = GetMesh()->CreateDynamicMaterialInstance(0, material);
		dynamicMaterial->SetVectorParameterValue("BodyColor", AlternateColour);
		GetMesh()->SetMaterial(0, dynamicMaterial);
	}

	// Bind combat phase start delegate
	if (HasAuthority())
	{
		AConquestGameState* conquestGameState = (AConquestGameState*)GetWorld()->GetGameState();
		conquestGameState->CombatPhase_OnStart.AddDynamic(this, &AConquestUnit::RespondToCombatPhaseBegin);
	}

	// Disable until combat phase
	SetActorTickEnabled(false);
}

// Called every frame
void AConquestUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (HasAuthority())
	{
		if (bIsDead)
		{
			return;
		}

		if (TargetingComponent->AcquireTargetEnemy())
		{
			// Move towards enemy
			const FVector& enemyLocation = TargetingComponent->GetTargetEnemy()->GetActorLocation();
			if (FVector::Dist(enemyLocation, GetActorLocation()) <= AttackRange)
			{
				// Attack
				FaceTargetEnemy(DeltaTime);
				AttackComponent->AttemptAttack(TargetingComponent->GetTargetEnemy());
			}
			else
			{
				MovementComponent->MoveToEnemy(enemyLocation);
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

bool AConquestUnit::IsDead()
{
	return bIsDead;
}

void AConquestUnit::RespondToCombatPhaseBegin()
{
	UE_LOG(LogConquest, Log, TEXT("COMBAT BEGIN: %s"), *GetNameSafe(this));
	SetActorTickEnabled(true);
}

void AConquestUnit::FaceTargetEnemy(float DeltaTime)
{
	// Face enemy
	FVector targetDirection = TargetingComponent->GetTargetEnemy()->GetActorLocation() - GetActorLocation();
	targetDirection.Normalize(SMALL_NUMBER);
	FRotator newRotation = FMath::RInterpTo(GetActorRotation(), targetDirection.Rotation(), DeltaTime, 1.0);
	SetActorRotation(newRotation);
}

void AConquestUnit::DeathBegin()
{
	if (HasAuthority())
	{
		bIsDead = true;
		OnRep_bIsDead();
	}
}

void AConquestUnit::OnRep_bIsDead()
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
