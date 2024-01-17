// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "CharacterBase.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

UCLASS(config=Game)
class ACharacterBase : public ACharacter
{
	GENERATED_BODY()

public:
	ACharacterBase();

protected:
	virtual void BeginPlay();

protected:
	/** Called for movement input */
	virtual void Move(const FInputActionValue& Value);

//====CharacterStat, CharacterState============
protected:
	/** Character HP **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Stat")
		int32 CharacterMaxHP;

private:
	//** Character HP **/
	int32 CharacterCurrHP;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		int32 GetCurrentHP() { return CharacterCurrHP; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		int32 GetMaxHP() { return CharacterMaxHP; }
};

