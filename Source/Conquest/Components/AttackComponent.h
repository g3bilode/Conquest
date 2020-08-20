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
	void AttemptAttack();
	/* Deal damage to given actor. */
	void DealDamage(AActor* TargetActor);


	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Attack")
	float BaseDamage;
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Attack")
	float AttackCooldown;


private:

	/* Play the attack animation. */
	//UFUNCTION(NetMulticast, Unreliable)
	UFUNCTION(NetMulticast, Reliable)
	void PlayAttackAnim();
	virtual void PlayAttackAnim_Implementation();

	/* Callback when cooldown timer runs out. */
	void OnAttackCooldownExpired();


	/* Timer handle to manage attack cooldown */
	FTimerHandle AttackCooldownTimerHandle;
	/* Is our attack on cooldown? */
	bool bIsOnCooldown;
		
};