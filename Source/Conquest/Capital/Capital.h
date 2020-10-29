// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interfaces/ConquestSelectableInterface.h"
#include "../Interfaces/TargeterInterface.h"
#include "AbilitySystemInterface.h"
#include "Capital.generated.h"

UCLASS()
class CONQUEST_API ACapital : public AActor, public ITargeterInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACapital();

	/* Return AbilitySystemComponent. From IAbilitySystemInterface. */
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

protected:
	void BeginPlay() override;

private:
	UPROPERTY()
	/* Root component */
	class USceneComponent* RootSceneComponent;
	/* Attack Component*/
	UPROPERTY(VisibleAnywhere, Category = "Attack")
	class UAttackComponent* AttackComponent;
	/* Targeting Component*/
	UPROPERTY(VisibleAnywhere, Category = "Attack")
	class UTargetingComponent* TargetingComponent;
	/* Health Component*/
	UPROPERTY(VisibleAnywhere, Category = "Health")
	class UHealthComponent* HealthComponent;
	/* Ability System Component */
	UPROPERTY(VisibleAnywhere, Category = "CASC")
	class UConquestAbilitySystemComponent* AbilitySystemComponent;

	/* Attributes */
	UPROPERTY()
	class UConquestAttributeSet* Attributes;


	/* Initialize attribute set. */
	virtual void InitializeAttributes();

	/* Assign default abilities. */
	virtual void GiveAbilities_Auth();

	// Attribute change callbacks
	virtual void OnHealthChanged(const struct FOnAttributeChangeData& Data);
	
public:
	/* Team Index */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Team")
	int32 TeamIndex;
	
	/* Attribute Effect to initialize attribute defaults */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "CASC")
	TSubclassOf<class UGameplayEffect> DefaultAttributeEffect;

	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool IsTargetEnemy_Implementation(AActor* OtherActor) override;

	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

};
