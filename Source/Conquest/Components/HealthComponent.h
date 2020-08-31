// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CONQUEST_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UHealthComponent();

	/* Return max possible health. */
	float GetMaxHealth() const;
	/* Return current health. */
	float GetCurrentHealth() const;
	/* Return percent of current health. */
	float GetHealthPercent() const;
	/* Take health damage. */
	void TakeDamage(float Damage);
	/* Return true if dead. */
	UFUNCTION(BlueprintCallable)
	bool IsDead() const;


	/* Base health for actor. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Health")
	float BaseHealth;

private:
	/* Actor's current health. */
	UPROPERTY(Replicated)
	float CurrentHealth;
	/* Is actor dead? */
	UPROPERTY(Replicated)
	bool bIsDead;
public:
	void BeginPlay() override;

};
