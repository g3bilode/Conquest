// Fill out your copyright notice in the Description page of Project Settings.

#include "ConquestUnit.h"
#include "../Conquest.h"
#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"
#include "../Components/UnitMovementComponent.h"
#include "../GameState/ConquestGameState.h"


const FLinearColor AConquestUnit::AlternateColour(0.29f, 0.57f, 0.79f);

// Sets default values
AConquestUnit::AConquestUnit()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// Setup Collision Spheres
	AggroSphere = CreateDefaultSubobject<USphereComponent>("AggroSphere");
	AggroSphere->SetupAttachment(RootComponent);
	AggroSphere->OnComponentBeginOverlap.AddDynamic(this, &AConquestUnit::OnAggroCollision);

	// Setup UnitMovementCompontent
	MovementComponent = CreateDefaultSubobject<UUnitMovementComponent>("MovementComponent");

	AggroSphereRadius = 300.0f;
	AttackRange = 180.0f;
	BaseDamage = 20.0f;
	AttackCooldown = 1.0f;
	Health = 100.0f;

	bIsDead = false;
}

void AConquestUnit::PostInitProperties()
{
	Super::PostInitProperties();
	AggroSphere->InitSphereRadius(AggroSphereRadius);
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

		if (AcquireTargetEnemy())
		{
			// Move towards enemy
			const FVector& enemyLocation = TargetEnemy->GetActorLocation();
			if (FVector::Dist(enemyLocation, GetActorLocation()) <= AttackRange)
			{
				// Attack
				FaceTargetEnemy(DeltaTime);
				AttackTargetEnemy();
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

void AConquestUnit::DealDamage()
{
	if (IsValid(TargetEnemy))
	{
		TargetEnemy->TakeDamage(BaseDamage, FDamageEvent(), GetController(), this);
	}
	GetWorld()->GetTimerManager().SetTimer(AttackCooldownTimerHandle, this, &AConquestUnit::OnAttackCooldownExpired, AttackCooldown, false);
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

void AConquestUnit::OnAggroCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	// TODO: This may only need to be done on local client?
	// Find a new target
	AConquestUnit* OtherConquestUnit = Cast<AConquestUnit>(OtherActor);
	if (IsValid(OtherConquestUnit))
	{
		// Found Unit
		if (GetNameSafe(OtherComp) == FString("CollisionCylinder"))
		{
			if (IsTargetEnemy(OtherConquestUnit))
			{
				KnownEnemies.Add(OtherConquestUnit);
			}
		}
	}
}

void AConquestUnit::OnAttackCooldownExpired()
{
	bIsOnCooldown = false;
}

void AConquestUnit::SetTargetEnemy(AConquestUnit* EnemyConquestUnit)
{
	TargetEnemy = EnemyConquestUnit;
	UE_LOG(LogConquest, Log, TEXT("TARGET ACQUIRED: %s"), *GetNameSafe(EnemyConquestUnit));
}

bool AConquestUnit::AcquireTargetEnemy()
{
	if (!IsValid(TargetEnemy) || TargetEnemy->IsDead())
	{
		// Find new enemy
		if (KnownEnemies.Num() > 0)
		{
			for (int32 i=KnownEnemies.Num()-1; i >= 0; i--)
			{
				AConquestUnit* knownEnemy = KnownEnemies[i];
				if (IsValid(knownEnemy) && !knownEnemy->IsDead())
				{
					// New target found
					SetTargetEnemy(knownEnemy);
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

void AConquestUnit::AttackTargetEnemy()
{
	if (!bIsOnCooldown)
	{
		bIsOnCooldown = true;
		PlayAttackAnim();
	}
}

void AConquestUnit::FaceTargetEnemy(float DeltaTime)
{
	// Face enemy
	FVector targetDirection = TargetEnemy->GetActorLocation() - GetActorLocation();
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

void AConquestUnit::PlayAttackAnim_Implementation()
{
	PlayAnimMontage(AttackMontage);
}

bool AConquestUnit::IsEnemyInMyLane(AConquestUnit* ConquestUnit)
{
	// TODO: Improve this?
	// Difference in lane index between teams (ie: 0 = 2)
	const int32 laneDiff = 2;
	return (LaneIndex + ConquestUnit->LaneIndex) == laneDiff;
}

bool AConquestUnit::IsTargetEnemy(AConquestUnit* ConquestUnit)
{
	return (ConquestUnit->TeamIndex != TeamIndex) && IsEnemyInMyLane(ConquestUnit);
}
