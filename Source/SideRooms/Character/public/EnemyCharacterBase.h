// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.h"
#include "InputActionValue.h"
#include "EnemyCharacterBase.generated.h"

UCLASS(config=Game)
class AEnemyCharacterBase : public ACharacterBase
{
	GENERATED_BODY()

//====== Basic Event ================
public:
	AEnemyCharacterBase();

protected:
	virtual void BeginPlay();

	virtual float TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser) override;

//======== Input / Action ============================
protected:
	/** Called for movement input */
	virtual void Move(const FInputActionValue& Value);

	virtual void Sprint(const FInputActionValue& Value);

	virtual void StopSprint(const FInputActionValue& Value);

public:
	virtual void EnableMovement() override;

	virtual void DisableMovement() override;

//====== Asset ================
protected:
	UPROPERTY(EditDefaultsOnly, Category = "Gameplay|Asset|Animation")
		class UAnimMontage* LeftLegHitMontage;	

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay|Asset|Animation")
		class UAnimMontage* RightLegHitMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay|Asset|Animation")
		class UAnimMontage* NormalHitMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay|Asset|Animation")
		class UAnimMontage* HeadHitMontage;

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay|Asset|Animation")
		class UAnimMontage* KnockDownMontage;
};