// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "ConquestUnit.generated.h"

UCLASS()
class CONQUEST_API AConquestUnit : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AConquestUnit();

	// Setup things driven by BP properties
	void PostInitProperties() override;

	float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FVector GetTargetDestination() const;

	void ProgressNextDestination();

	void SetTargetDestination(FVector newLocation);

	void SetLaneDestinations(const TArray<FVector>& InLaneDestinations);

	UFUNCTION(BlueprintCallable)
	void DealDamage();

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Team")
	FName TeamName;

	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Team")
	int32 TeamIndex;

	UPROPERTY(Replicated)
	int32 LaneIndex;

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cost")
	int32 PurchaseCost;
	
	
	// STATS
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Attack")
	float AggroSphereRadius;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Attack")
	float AttackRange;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Attack")
	float BaseDamage;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Attack")
	float AttackCooldown;
	UPROPERTY(Replicated, BlueprintReadWrite, EditDefaultsOnly, Category = "Health")
	float Health;

	// ANIMS
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Anim")
	UAnimMontage* AttackMontage;


private:

	void MoveToDestination(const FVector& Destination);
	bool HasArrivedAtDestination();
	UFUNCTION()
	void OnAggroCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
	void OnAttackCooldownExpired();
	bool IsTargetEnemy(AConquestUnit* ConquestUnit);
	bool IsEnemyInMyLane(AConquestUnit* ConquestUnit);
	void SetTargetEnemy(AConquestUnit* EnemyConquestUnit);
	void AttackTargetEnemy();

	UPROPERTY(Replicated)
	FVector TargetDestination;
	UPROPERTY()
	bool IsAtDestination;
	/* List of destinations for unit's lane */
	TArray<FVector> LaneDestinations;
	/* Current location in lane */
	int8 CurrentDestinationIndex;
	/* Alternate unit colour */
	static const FLinearColor AlternateColour;


	// Attack
	
	/* Currently targeted enemy*/
	UPROPERTY()
	AConquestUnit* TargetEnemy;
	/* Aggro Trigger Sphere */
	UPROPERTY()
	class USphereComponent* AggroSphere;
	/* Timer handle to manage attack cooldown */
	FTimerHandle AttackCooldownTimerHandle;
	/* Is our attack on cooldown? */
	bool bIsOnCooldown;
};
