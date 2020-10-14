// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AttackComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CONQUEST_API UAttackComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAttackComponent();

public:

	/* Attack animation montage. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Anim")
	UAnimMontage* AttackMontage;

	/* Attack target enemy. */
	void AttemptAttack(AActor* TargetActor);
	/* Deal damage to given actor. */
	void DealDamage();
	/* Return true if target in range of attack. */
	bool IsTargetInRange(AActor* TargetActor);
	/* Get Target Enemy. */
	UFUNCTION(BlueprintCallable)
	AActor* GetCurrentTarget() const;

	/* Play the attack animation. */
	UFUNCTION(BlueprintCallable, NetMulticast, Unreliable)
	void PlayAttackAnim();
	virtual void PlayAttackAnim_Implementation();


	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Attack")
	float BaseDamage;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Attack")
	float AttackCooldown;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Attack")
	float AttackRange;

	/* Default Abilities */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "CASC")
	TArray<TSubclassOf<class UConquestGameplayAbility>> DefaultAbilities;


private:

	/* Callback when cooldown timer runs out. */
	void OnAttackCooldownExpired();

	/* Get (and set) owning actor. */
	AActor* GetOwningActor();


	UPROPERTY()
	/* Component owner */
	AActor* _OwningActor;
	/* Target actor to attack. */
	UPROPERTY()
	AActor* _TargetActor;
	/* Timer handle to manage attack cooldown */
	FTimerHandle AttackCooldownTimerHandle;
	/* Is our attack on cooldown? */
	bool bIsOnCooldown;
		
};
