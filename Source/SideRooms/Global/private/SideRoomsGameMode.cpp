// Copyright Epic Games, Inc. All Rights Reserved.

#include "../public/SideRoomsGameMode.h"
#include "UObject/ConstructorHelpers.h"
#include "SocketSubsystem.h"

ASideRoomsGameMode::ASideRoomsGameMode()
	: Super()
{
	
}

FString ASideRoomsGameMode::GetMyIpAddress()
{
	FString IpAddr("NONE");
	bool canBind = false;
	TSharedRef<FInternetAddr> LocalIp = ISocketSubsystem::Get(PLATFORM_SOCKETSUBSYSTEM)->GetLocalHostAddr(*GLog, canBind);
	if (LocalIp->IsValid()) 
	{ 
		IpAddr  = LocalIp->ToString(false); 
	}       
	return IpAddr;
}
