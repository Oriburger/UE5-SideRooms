// Fill out your copyright notice in the Description page of Project Settings.


#include "../public/SideRoomsGameState.h"
#include "GameFramework/PlayerState.h"
#include "../../Character/public/CharacterBase.h"
#include "../../Character/public/EnemyCharacterBase.h"

void ASideRoomsGameState::InitGame()
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

}

void ASideRoomsGameState::InitMonster()
{
	AEnemyCharacterBase* enemy = GetWorld()->SpawnActor<AEnemyCharacterBase>(EnemyCharacterClass, FVector(0.0f, 0.0f, 100.0f), FRotator());
	checkf(enemy != nullptr, TEXT("ASideRoomsGameState::InitGame -> Failed to spawn enemy character"));
	EnemyCharacterRefList.Add(enemy);
}

void ASideRoomsGameState::InitPlayer()
{
	for (auto& playerState : PlayerArray)
	{
		if (playerState == nullptr) continue; 
		Cast<ACharacterBase>(playerState->GetPawn())->EnableMovement();
	}
}

void ASideRoomsGameState::TryUpdateMissionCount(int32 AddValue)
{
	if (HasAuthority())
	{
		UpdateMissionCount(AddValue);
	}
	else
	{
		ServerRPCUpdateMissionCount(AddValue);
	}
}

void ASideRoomsGameState::UpdateMissionCount_Implementation(int AddValue)
{
	CurrentMissionCount += AddValue;
}

void ASideRoomsGameState::ServerRPCUpdateMissionCount_Implementation(int AddValue)
{
	if (AddValue != 1) return;
	UpdateMissionCount(AddValue);
	//GetWorld()->GetAuthGameMode()->CheckMissionComplete(CurrentMissionCount);
}