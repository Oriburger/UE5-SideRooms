// Fill out your copyright notice in the Description page of Project Settings.


#include "../public/SideRoomsGameState.h"
#include "../public/SideRoomsGameMode.h"
#include "GameFramework/PlayerState.h"
#include "Kismet/GameplayStatics.h"
#include "../../Character/public/CharacterBase.h"
#include "../../Character/public/EnemyCharacterBase.h"

void ASideRoomsGameState::InitGame_Implementation()
{
	// Map & Mission Init
	InitMap();

	// Mob Init
	InitMonster();

	// Player Location Init
	InitPlayer();
}

void ASideRoomsGameState::InitMap()
{
	//Randomize Props, Decals, ETC
}

void ASideRoomsGameState::InitMonster()
{
	//for playerArray enemy spawn
	AEnemyCharacterBase* enemy = GetWorld()->SpawnActor<AEnemyCharacterBase>(EnemyCharacterClass, FVector(0.0f, 0.0f, 100.0f), FRotator());
	checkf(enemy != nullptr, TEXT("ASideRoomsGameState::InitGame -> Failed to spawn enemy character"));
	EnemyCharacterRefList.Add(enemy);
	enemy->EnableMovement();
}

void ASideRoomsGameState::InitPlayer()
{
	for (auto& playerState : PlayerArray)
	{
		if (playerState == nullptr) continue; 
		Cast<ACharacterBase>(playerState->GetPawn())->EnableMovement();
	}

	//Player Set Location
}

void ASideRoomsGameState::TryUpdateMissionCount()
{
	if (HasAuthority())
	{
		IncreaseMissionCount();
	}
	else
	{
		ServerRPCIncreaseMissionCount();
	}
}

void ASideRoomsGameState::IncreaseMissionCount_Implementation()
{
	CurrentMissionCount += 1;
}

void ASideRoomsGameState::ServerRPCIncreaseMissionCount_Implementation()
{
	IncreaseMissionCount();
	
	ASideRoomsGameMode* gamemode = Cast<ASideRoomsGameMode>(GetWorld()->GetAuthGameMode());
	if (gamemode)
	{
		gamemode->CheckMissionComplete(CurrentMissionCount);
	}
}