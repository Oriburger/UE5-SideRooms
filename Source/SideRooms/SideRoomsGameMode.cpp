// Copyright Epic Games, Inc. All Rights Reserved.

#include "SideRoomsGameMode.h"
#include "SideRoomsCharacter.h"
#include "UObject/ConstructorHelpers.h"

ASideRoomsGameMode::ASideRoomsGameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPerson/Blueprints/BP_FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

}
