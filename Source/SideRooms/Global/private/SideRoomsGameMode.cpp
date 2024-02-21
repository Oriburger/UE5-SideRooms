// Copyright Epic Games, Inc. All Rights Reserved.

#include "../public/SideRoomsGameMode.h"
#include "../public/SideRoomsGameState.h"
#include "../../Character/public/SideRoomPlayerController.h"
#include "UObject/ConstructorHelpers.h"
#include "SocketSubsystem.h"

ASideRoomsGameMode::ASideRoomsGameMode() : Super()
{
	
}

void ASideRoomsGameMode::PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage)
{
	Super::PreLogin(Options, Address, UniqueId, ErrorMessage);
}

void ASideRoomsGameMode::PostLogin(APlayerController* NewPlayer)
{
	if (bHasBegun || PlayerControllerList.Num() >= 2)
	{
		Cast<ASideRoomPlayerController>(NewPlayer)->DisconnectGame();
	}

	Super::PostLogin(NewPlayer);

	//Add new player to list 
	PlayerControllerList.Add(NewPlayer);
}

void ASideRoomsGameMode::StartGame_Implementation()
{
	bHasBegun = true;

	//Initialize Game (Map Generator, Mob & Player Placing ..)
	GetGameState<ASideRoomsGameState>()->InitGame();
	
	//Set timer for updating difficulty
	GetWorldTimerManager().SetTimer(ElapsedTimer, this, &ASideRoomsGameMode::UpdateDifficulty, 
									DifficultyUpdateInterval, true);
}

void ASideRoomsGameMode::FinishGame_Implementation()
{
	float totalElapsedTime = GetWorldTimerManager().GetTimerElapsed(ElapsedTimer);
	GetWorldTimerManager().ClearTimer(ElapsedTimer);
}

void ASideRoomsGameMode::UpdateDifficulty()
{
	CurrentDifficulty += 1;
	if (DifficultyInfoList.IsValidIndex(CurrentDifficulty))
	{
		//UpdateAIStat.Execute(DifficultyInfoList[NewDifficulty]);
	}
}

FString ASideRoomsGameMode::GetMyIpAddress()
{
	FString IpAddr("NONE");
	bool canBind = false;
	TSharedRef<FInternetAddr> LocalIp = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, canBind);
	if (LocalIp->IsValid())
	{
		IpAddr = LocalIp->ToString(false);
	}
	return IpAddr;
}