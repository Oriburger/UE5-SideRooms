// Copyright Epic Games, Inc. All Rights Reserved.

#include "../public/EnemyCharacterBase.h"
#include "../public/CharacterBase.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "GameFramework/Controller.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "AIController.h"
#include "BrainComponent.h"
#include "GameFramework/CharacterMovementComponent.h"


//////////////////////////////////////////////////////////////////////////
// AEnemyCharacterBase

AEnemyCharacterBase::AEnemyCharacterBase()
{
	Tags.Add("Enemy");
}

void AEnemyCharacterBase::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();
}

float AEnemyCharacterBase::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	//if(NormalHitMontage != nullptr)
	//	PlayAnimMontage(NormalHitMontage);

	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
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

void AEnemyCharacterBase::EnableMovement()
{
	Super::EnableMovement();
	if(IsValid(Controller)) Cast<AAIController>(Controller)->GetBrainComponent()->RestartLogic();
}

void AEnemyCharacterBase::DisableMovement()
{
	Super::DisableMovement();
	if (IsValid(Controller)) Cast<AAIController>(Controller)->GetBrainComponent()->StopLogic("");
}
