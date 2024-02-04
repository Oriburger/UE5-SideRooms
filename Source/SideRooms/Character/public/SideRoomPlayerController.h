// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "SideRoomPlayerController.generated.h"

/**
 * 
 */
UCLASS()
class SIDEROOMS_API ASideRoomPlayerController : public APlayerController
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintImplementableEvent)
		void InitHUD();

};
