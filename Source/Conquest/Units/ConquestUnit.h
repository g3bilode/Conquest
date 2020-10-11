// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "../Interfaces/TargeterInterface.h"
#include "AbilitySystemInterface.h"
//#include "GameplayEffectTypes.h"
#include "GameFramework/Character.h"
#include "ConquestUnit.generated.h"


UCLASS()
class CONQUEST_API AConquestUnit : public ACharacter, public ITargeterInterface, public IAbilitySystemInterface
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AConquestUnit();
	
	UAbilitySystemComponent* GetAbilitySystemComponent() const override;

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
	/* Display bounty. */
	void DisplayBounty() const;

	/* Unit's team index. */
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Team")
	int32 TeamIndex;

	/* Unit's team lane. */
	UPROPERTY(Replicated)
	int32 LaneIndex;

	/* Purchase cost for this unit. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Cost")
	int32 PurchaseCost;
	/* Gold gain from opponent on kill by unit. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Cost")
	int32 Bounty;

	/* Icon for this unit. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "UI")
	UTexture2D* Icon;

	/* Classes that this unit can evolve into. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ConquestUnit")
	TArray<TSubclassOf<class AConquestUnit>> Evolutions;

	/* Base unit colour. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ConquestUnit")
	FLinearColor BaseColour;
	/* Alternate unit colour */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ConquestUnit")
	FLinearColor AlternateColour;

	/* Attribute Effect to initialize attribute defaults */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Attributes")
	TSubclassOf<class UGameplayEffect> DefaultAttributeEffect;

	/* Default Abilities */
	UPROPERTY(BlueprintReadOnly, EditDefaultsOnly, Category = "Attributes")
	TArray<TSubclassOf<class UConquestGameplayAbility>> DefaultAbilities;

	// ANIMS
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Anim")
	UAnimMontage* DeathMontage;

private:

	/* Initialize attribute set. */
	virtual void InitializeAttributes();

	/* Assign default abilities. */
	virtual void GiveAbilities_Auth();

	/* Respond to combat phase begin delegate. */
	UFUNCTION()
	void RespondToCombatPhaseBegin();

	/* Return true if given enemy is in our lane. */
	bool IsEnemyInMyLane(AConquestUnit* ConquestUnit);
	/* Smoothly face target enemy. */
	void FaceTargetEnemy(float DeltaTime);
	/* Begin death process. */
	void DeathBegin();
	/* Play the death anim on server/clients. */
	UFUNCTION(NetMulticast, Unreliable)
	void PlayDeathAnim();
	virtual void PlayDeathAnim_Implementation();

	/* Display bounty for killing player. */
	void DisplayBountyForTeam(int32 KillerTeamIndex) const;

	/* Unit Movement Component*/
	UPROPERTY(VisibleAnywhere, Category = "Movement")
	class UUnitMovementComponent* MovementComponent;
	/* Attack Component*/
	UPROPERTY(VisibleAnywhere, Category = "Attack")
	class UAttackComponent* AttackComponent;
	/* Targeting Component*/
	UPROPERTY(VisibleAnywhere, Category = "Attack")
	class UTargetingComponent* TargetingComponent;
	/* Health Component*/
	UPROPERTY(VisibleAnywhere, Category = "Health")
	class UHealthComponent* HealthComponent;
	/* Resource drip component. */
	UPROPERTY(VisibleAnywhere, Category = "Cost")
	class UResourceDripComponent* ResourceDripComponent;
	/* Ability System Component */
	UPROPERTY(VisibleAnywhere, Category = "Abilities")
	class UConquestAbilitySystemComponent* AbilitySystemComponent;

	/* Attributes */
	UPROPERTY()
	class UConquestAttributeSet* Attributes;

};
