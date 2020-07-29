// Fill out your copyright notice in the Description page of Project Settings.

#include "ConquestUnit.h"
#include "Conquest.h"
#include "UnrealNetwork.h"
#include "Components/SphereComponent.h"


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

	CurrentDestinationIndex = -1;
}

void AConquestUnit::PostInitProperties()
{
	Super::PostInitProperties();
	AggroSphere->InitSphereRadius(AggroSphereRadius);
}

void AConquestUnit::GetLifetimeReplicatedProps(TArray<FLifetimeProperty> &OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(AConquestUnit, TeamName);
	DOREPLIFETIME(AConquestUnit, TeamIndex);
	DOREPLIFETIME(AConquestUnit, TargetDestination);
	DOREPLIFETIME(AConquestUnit, LaneIndex);
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

	ProgressNextDestination();
}

// Called every frame
void AConquestUnit::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if (IsValid(TargetEnemy))
	{
		// Move towards enemy
		const FVector& enemyLocation = TargetEnemy->GetActorLocation();
		if (FVector::Dist(enemyLocation, GetActorLocation()) < 90.0f)
		{
			// Attack
			AttackTargetEnemy();
		}
		else
		{
			MoveToDestination(enemyLocation);
		}
	}
	else if (!IsAtDestination)
	{
		// Move towards next point
		MoveToDestination(TargetDestination);
	}
}

FVector AConquestUnit::GetTargetDestination() const
{
	return TargetDestination;
}

void AConquestUnit::ProgressNextDestination()
{
	CurrentDestinationIndex++;
	if (CurrentDestinationIndex < LaneDestinations.Num())
	{
		SetTargetDestination(LaneDestinations[CurrentDestinationIndex]);
	}
	else
	{
		UE_LOG(LogConquest, Log, TEXT("I have arrived at final destination!"));
	}
}

void AConquestUnit::SetTargetDestination(FVector newLocation)
{
	TargetDestination = newLocation;
	IsAtDestination = false;
}

void AConquestUnit::SetLaneDestinations(const TArray<FVector>& InLaneDestinations)
{
	LaneDestinations = InLaneDestinations;
}

void AConquestUnit::MoveToDestination(const FVector& Destination)
{
	FVector targetDirection = Destination - GetActorLocation();
	targetDirection.Normalize(SMALL_NUMBER);

	SetActorRotation(FVector(targetDirection.X, targetDirection.Y, 0).Rotation());
	AddMovementInput(targetDirection);
	if (HasArrivedAtDestination())
	{
		UE_LOG(LogConquest, Log, TEXT("I have arrived!"));
		IsAtDestination = true;
		ProgressNextDestination();
	}
}

bool AConquestUnit::HasArrivedAtDestination()
{
	return FVector::DistXY(TargetDestination, GetActorLocation()) < 1.0f;
}

void AConquestUnit::OnAggroCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult)
{
	// TODO: This may only need to be done on local client?
	if (!IsValid(TargetEnemy))
	{
		// Find a new target
		AConquestUnit* OtherConquestUnit = Cast<AConquestUnit>(OtherActor);
		if (IsValid(OtherConquestUnit))
		{
			// Found Unit
			if (GetNameSafe(OtherComp) == FString("CollisionCylinder"))
			{
				if (IsTargetEnemy(OtherConquestUnit))
				{
					SetTargetEnemy(OtherConquestUnit);
				}
			}
		}
	}
}

void AConquestUnit::SetTargetEnemy(AConquestUnit* EnemyConquestUnit)
{
	TargetEnemy = EnemyConquestUnit;
	UE_LOG(LogConquest, Log, TEXT("TARGET ACQUIRED: %s"), *GetNameSafe(EnemyConquestUnit));
}

void AConquestUnit::AttackTargetEnemy()
{
	UE_LOG(LogConquest, Log, TEXT("HYYYAAAA!"));
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
