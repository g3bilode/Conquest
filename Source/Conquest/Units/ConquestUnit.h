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

	void SetLaneDestinations(const TArray<FVector>& InLaneDestinations);

	UFUNCTION(BlueprintCallable)
	void DealDamage();
	UFUNCTION(BlueprintCallable)
	void DeathEnd();
	UFUNCTION(BlueprintCallable)
	bool IsDead();

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
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Anim")
	UAnimMontage* DeathMontage;


private:

	UFUNCTION()
	/* Respond to combat phase begin delegate. */
	void RespondToCombatPhaseBegin();

	UFUNCTION()
	/* Callback when unit overlaps our aggro sphere. */
	void OnAggroCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
	/* Callback when cooldown timer runs out. */
	void OnAttackCooldownExpired();
	/* Return true if given unit is enemy we can target. */
	bool IsTargetEnemy(AConquestUnit* ConquestUnit);
	/* Return true if given enemy is in our lane. */
	bool IsEnemyInMyLane(AConquestUnit* ConquestUnit);
	/* Set new target enemy. */
	void SetTargetEnemy(AConquestUnit* EnemyConquestUnit);
	/* Find a new TargetEnemy. Returns true if found. */
	bool AcquireTargetEnemy();
	/* Attack target enemy. */
	void AttackTargetEnemy();
	/* Smoothly face target enemy. */
	void FaceTargetEnemy(float DeltaTime);
	/* Begin death process. */
	void DeathBegin();

	UFUNCTION()
	virtual void OnRep_bIsDead();

	UFUNCTION(NetMulticast, Unreliable)
	void PlayAttackAnim();
	virtual void PlayAttackAnim_Implementation();

	/* Alternate unit colour */
	static const FLinearColor AlternateColour;


	/* Unit Movement Component*/
	UPROPERTY()
	class UUnitMovementComponent* MovementComponent;

	// Attack
	
	/* Currently targeted enemy*/
	UPROPERTY()
	AConquestUnit* TargetEnemy;
	/* Known enemies */
	UPROPERTY()
	TArray<AConquestUnit*> KnownEnemies;
	/* Aggro Trigger Sphere */
	UPROPERTY()
	class USphereComponent* AggroSphere;
	/* Timer handle to manage attack cooldown */
	FTimerHandle AttackCooldownTimerHandle;
	/* Is our attack on cooldown? */
	bool bIsOnCooldown;
	/* Are we dead? */
	UPROPERTY(ReplicatedUsing=OnRep_bIsDead)
	bool bIsDead;
};
