// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "ConquestCharacter.h"

#include "../Conquest.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/SpringArmComponent.h"
#include "HeadMountedDisplayFunctionLibrary.h"
#include "Materials/Material.h"
#include "Engine/World.h"
#include "Components/SphereComponent.h"
#include "Engine/Engine.h"
#include "../Interfaces/ConquestSelectableInterface.h"

AConquestCharacter::AConquestCharacter()
	:Super()
{
	// Don't rotate character to camera direction
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = true;
	bUseControllerRotationRoll = false;

	// Disable Crouch and Jump
	bAddDefaultMovementBindings = false;

	// Create a camera boom...
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->SetUsingAbsoluteRotation(false); // Rotate Arm relative to character
	CameraBoom->TargetArmLength = 800.f;
	CameraBoom->SetRelativeRotation(FRotator(-30.f, 0.f, 0.f));
	CameraBoom->bDoCollisionTest = false; // Don't want to pull camera in when it collides with level

	// Move camera boom with character only on Yaw rotation
	CameraBoom->bUsePawnControlRotation = false;
	CameraBoom->bInheritPitch = false;
	CameraBoom->bInheritRoll = false;
	CameraBoom->bInheritYaw = true;

	// Enable camera lag
	CameraBoom->bEnableCameraLag = true;
	CameraBoom->bEnableCameraRotationLag = true;

	// Disable collisions
	//GetCollisionComponent()->bGenerateOverlapEvents = false;
	GetCollisionComponent()->SetCollisionProfileName("NoCollision");


	// Create a camera...
	TopDownCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("TopDownCamera"));
	TopDownCameraComponent->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	TopDownCameraComponent->bUsePawnControlRotation = false; // Camera does not rotate relative to arm
}

void AConquestCharacter::ChangeCameraArmLength(float changeValue)
{
	CameraBoom->TargetArmLength += changeValue * 100.0f;
}

void AConquestCharacter::RotateCameraArm(FRotator rotation)
{
	CameraBoom->AddRelativeRotation(rotation);
}

void AConquestCharacter::MoveCharacterForward(float changeValue)
{
	AddMovementInput(GetActorForwardVector(), changeValue);
}

void AConquestCharacter::MoveCharacterRight(float changeValue)
{
	AddMovementInput(GetActorRightVector(), changeValue);
}
