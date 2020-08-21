// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Interfaces/TargeterInterface.h"
#include "GameFramework/Character.h"
#include "ConquestUnit.generated.h"


UCLASS()
class CONQUEST_API AConquestUnit : public ACharacter, public ITargeterInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AConquestUnit();

	float TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

	bool IsTargetEnemy_Implementation(AActor* OtherActor) override;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	void SetLaneDestinations(const TArray<FVector>& InLaneDestinations);

	UFUNCTION(BlueprintCallable)
	void OnAttackAnimHit();
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
	UPROPERTY(Replicated, BlueprintReadWrite, EditDefaultsOnly, Category = "Health")
	float Health;

	// ANIMS
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Anim")
	UAnimMontage* DeathMontage;


private:

	UFUNCTION()
	/* Respond to combat phase begin delegate. */
	void RespondToCombatPhaseBegin();

	/* Return true if given enemy is in our lane. */
	bool IsEnemyInMyLane(AConquestUnit* ConquestUnit);
	/* Smoothly face target enemy. */
	void FaceTargetEnemy(float DeltaTime);
	/* Begin death process. */
	void DeathBegin();

	UFUNCTION()
	virtual void OnRep_bIsDead();

	/* Alternate unit colour */
	static const FLinearColor AlternateColour;


	/* Unit Movement Component*/
	UPROPERTY()
	class UUnitMovementComponent* MovementComponent;
	/* Attack Component*/
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	class UAttackComponent* AttackComponent;
	/* Targeting Component*/
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	class UTargetingComponent* TargetingComponent;


	/* Are we dead? */
	UPROPERTY(ReplicatedUsing=OnRep_bIsDead)
	bool bIsDead;
};
