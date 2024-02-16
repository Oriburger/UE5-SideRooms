// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SphereComponent.h"
#include "Interactable.h"
#include "TP_PickUpComponent.generated.h"

// Declaration of the delegate that will be called when someone picks this up
// The character picking this up is the parameter sent with the notification
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnPickUp, ACharacterBase*, PickUpCharacter);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnBeginFocus);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnEndFocus);

UCLASS(Blueprintable, BlueprintType, ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class SIDEROOMS_API UTP_PickUpComponent : public USphereComponent, public IInteractable
{
	GENERATED_BODY()

public:
	UTP_PickUpComponent();

protected:
	/** Called when the game starts */
	virtual void BeginPlay() override;

public:
	/** Delegate to whom anyone can subscribe to receive this event */
	UPROPERTY(BlueprintAssignable, Category = "Interaction")
		FOnPickUp OnPickUp;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
		FOnBeginFocus OnBeginFocus;

	UPROPERTY(BlueprintAssignable, Category = "Interaction")
		FOnEndFocus OnEndFocus; 


//===== IInteractable ==============
public:
	// Function to interact with the object
	virtual void Interact(AActor* Interactor) override;

	//Set Actor to interactable
	virtual void Activate() override;

	//Set Actor to not interactable
	virtual void Deactivate() override;
};