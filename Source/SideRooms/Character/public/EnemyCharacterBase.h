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

public:
	AEnemyCharacterBase();

protected:
	virtual void BeginPlay();

protected:
	/** Called for movement input */
	virtual void Move(const FInputActionValue& Value);

	virtual void Sprint(const FInputActionValue& Value);

	virtual void StopSprint(const FInputActionValue& Value);
};