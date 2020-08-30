// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "../Interfaces/ConquestSelectableInterface.h"
#include "../Interfaces/TargeterInterface.h"
#include "Capital.generated.h"

UCLASS()
class CONQUEST_API ACapital : public AActor, public ITargeterInterface
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACapital();

	/* Team Index */
	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Team")
	int32 TeamIndex;


private:
	UPROPERTY()
	/* Root component */
	class USceneComponent* RootSceneComponent;
	/* Attack Component*/
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	class UAttackComponent* AttackComponent;
	/* Targeting Component*/
	UPROPERTY(EditDefaultsOnly, Category = "Attack")
	class UTargetingComponent* TargetingComponent;
	/* Health Component*/
	UPROPERTY(EditAnywhere, Category = "Health")
	class UHealthComponent* HealthComponent;
	
public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	bool IsTargetEnemy_Implementation(AActor* OtherActor) override;


	float TakeDamage(float DamageAmount, struct FDamageEvent const& DamageEvent, class AController* EventInstigator, AActor* DamageCauser) override;

};
