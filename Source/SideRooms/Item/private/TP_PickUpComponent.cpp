// Copyright Epic Games, Inc. All Rights Reserved.

#include "../public/TP_PickUpComponent.h"
#include "../../Character/public/MainCharacterBase.h"
#include "../../Character/public/CharacterBase.h"

UTP_PickUpComponent::UTP_PickUpComponent()
{
	SphereRadius = 5.0f;
}

void UTP_PickUpComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UTP_PickUpComponent::Interact(AActor* Interactor)
{
	if (!IsValid(Interactor) || !Interactor->ActorHasTag("Player")) return;
	OnPickUp.Broadcast(Cast<ACharacterBase>(Interactor));
}

void UTP_PickUpComponent::Activate()
{
	UE_LOG(LogTemp, Warning, TEXT("Activate"));
	OnBeginFocus.Broadcast();
}

void UTP_PickUpComponent::Deactivate()
{
	UE_LOG(LogTemp, Warning, TEXT("Deactivate"));
	OnEndFocus.Broadcast();
}