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

//==== Basic ===================
protected:
	virtual void OnPossess(APawn* InPawn) override;

public:
	UFUNCTION(Client, Reliable)
		void DisconnectGame();

	UFUNCTION(BlueprintImplementableEvent)
		void ActivateFadeEffect(bool bFadeOut = false);

//==== Widget ===================
protected:
	UFUNCTION(Client, Reliable)
		void InitWidget();	

	UPROPERTY(EditDefaultsOnly, Category = "Gameplay|Widget")
		TSubclassOf<class UUserWidget> DefaultWidgetClass;
};
