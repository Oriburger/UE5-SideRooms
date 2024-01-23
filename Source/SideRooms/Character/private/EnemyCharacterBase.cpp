// Copyright Epic Games, Inc. All Rights Reserved.

#include "../public/EnemyCharacterBase.h"
#include "../public/CharacterBase.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"


//////////////////////////////////////////////////////////////////////////
// AEnemyCharacterBase

AEnemyCharacterBase::AEnemyCharacterBase()
{
}

void AEnemyCharacterBase::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

void AEnemyCharacterBase::Move(const FInputActionValue& Value)
{
	Super::Move(Value);
}

void AEnemyCharacterBase::Sprint(const FInputActionValue& Value)
{
	Super::Sprint(Value);
}

void AEnemyCharacterBase::StopSprint(const FInputActionValue& Value)
{
	Super::StopSprint(Value);
}