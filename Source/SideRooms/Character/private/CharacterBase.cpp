// Copyright Epic Games, Inc. All Rights Reserved.

#include "../public/CharacterBase.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"


//////////////////////////////////////////////////////////////////////////
// ACharacterBase

ACharacterBase::ACharacterBase()
{
	//Init Character Stat
	CharacterCurrHP = CharacterMaxHP;

	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
}

void ACharacterBase::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void ACharacterBase::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ACharacterBase::Sprint(const FInputActionValue& Value)
{
	if (bIsSprinting) return;
	if (GetCharacterMovement()->GetCurrentAcceleration().IsNearlyZero()) return;

	UE_LOG(LogTemp, Warning, TEXT("Sprint#3"));

	bIsSprinting = true;
	GetCharacterMovement()->MaxWalkSpeed *= SprintMultipleVal;
}

void ACharacterBase::StopSprint(const FInputActionValue& Value)
{
	if (!bIsSprinting) return;
	
	UE_LOG(LogTemp, Warning, TEXT("Stop Sprint #4"));

	bIsSprinting = false;
	GetCharacterMovement()->MaxWalkSpeed /= SprintMultipleVal;
}