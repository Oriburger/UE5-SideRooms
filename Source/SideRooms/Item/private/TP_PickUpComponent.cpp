// Copyright Epic Games, Inc. All Rights Reserved.

#include "../public/TP_PickUpComponent.h"
#include "../../Character/public/MainCharacterBase.h"
#include "../../Character/public/CharacterBase.h"

UTP_PickUpComponent::UTP_PickUpComponent()
{
	// Setup the Sphere Collision
	SphereRadius = 32.f;
}

void UTP_PickUpComponent::BeginPlay()
{
	Super::BeginPlay();

	// Register our Overlap Event
	OnComponentBeginOverlap.AddDynamic(this, &UTP_PickUpComponent::OnSphereBeginOverlap);
}

void UTP_PickUpComponent::OnSphereBeginOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	// Checking if it is a First Person Character overlapping
	ACharacterBase* character = Cast<ACharacterBase>(OtherActor);

	// Only player character is able to pick up
	if(IsValid(character) && character->ActorHasTag("Player"))
	{
		// Notify that the actor is being picked up
		OnPickUp.Broadcast(character);

		// Unregister from the Overlap Event so it is no longer triggered
		OnComponentBeginOverlap.RemoveAll(this);
	}
}
