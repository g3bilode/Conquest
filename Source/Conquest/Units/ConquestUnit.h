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

	/* Unit's team index. */
	UPROPERTY(BlueprintReadOnly, Replicated, Category = "Team")
	int32 TeamIndex;

	/* Unit's team lane. */
	UPROPERTY(Replicated)
	int32 LaneIndex;

	/* Purchase cost for this unit. */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Cost")
	int32 PurchaseCost;

	/* Icon for this unit. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "UI")
	UTexture2D* Icon;

	/* Classes that this unit can evolve into. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "ConquestUnit")
	TArray<TSubclassOf<class AConquestUnit>> Evolutions;

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
	/* Play the death anim on server/clients. */
	UFUNCTION(NetMulticast, Unreliable)
	void PlayDeathAnim();
	virtual void PlayDeathAnim_Implementation();

	/* Alternate unit colour */
	static const FLinearColor AlternateColour;

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
};
