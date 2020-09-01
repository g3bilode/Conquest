// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetingComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class CONQUEST_API UTargetingComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UTargetingComponent();

protected:
	void OnRegister() override;

public:
	/* Return TargetEnemy. */
	AActor* GetTargetEnemy() const;
	/* Find a new TargetEnemy. Returns true if found. */
	bool AcquireTargetEnemy();

	/* Radius of aggro targeting sphere. */
	UPROPERTY(BlueprintReadWrite, EditDefaultsOnly, Category = "Attack")
	float AggroSphereRadius;

private:

	/* Callback when unit overlaps our aggro sphere. */
	UFUNCTION()
	void OnAggroCollision(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult &SweepResult);
	/* Is this a valid target? */
	bool IsValidTarget(AActor* TargetActor);
	
	/* Currently targeted enemy*/
	UPROPERTY()
	AActor* TargetEnemy;
	/* Known enemies */
	UPROPERTY()
	TArray<AActor*> KnownEnemies;
	/* Aggro Trigger Sphere */
	UPROPERTY()
	class USphereComponent* AggroSphere;

};
