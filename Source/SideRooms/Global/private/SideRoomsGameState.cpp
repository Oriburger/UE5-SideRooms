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

	// Player Location Init
	InitPlayer();

	// Mob Init
	InitMonster();
}

void ASideRoomsGameState::InitMap()
{
	// Spawn Location Vector Init
	SpawnLocationList.Add(FVector(325, 14840, 80));
	SpawnLocationList.Add(FVector(325, -14195, 80));
	SpawnLocationList.Add(FVector(-13075, -385, 80));
	SpawnLocationList.Add(FVector(15985, -385, 80));

	//Randomize Props, Decals, ETC

}

void ASideRoomsGameState::InitPlayer()
{
	FRandomStream randomVal;
	randomVal.GenerateNewSeed();

	for (auto& playerState : PlayerArray)
	{
		if (playerState == nullptr) continue;
		Cast<ACharacterBase>(playerState->GetPawn())->EnableMovement();

		int randomInt = (int)randomVal.RandRange(0, SpawnLocationList.Num() - 1);

		Cast<AActor>(playerState->GetPawn())->SetActorLocation(SpawnLocationList[randomInt], false);
		SpawnLocationList.RemoveAt(randomInt);
	}
}

void ASideRoomsGameState::InitMonster()
{
	float vectorX = 4000;
	float vectorY = 4000;

	FRandomStream randomVal;
	randomVal.GenerateNewSeed();

	for (auto& player : PlayerArray)
	{
		AEnemyCharacterBase* enemy = GetWorld()->SpawnActor<AEnemyCharacterBase>(EnemyCharacterClass, Cast<AActor>(player->GetPawn())->GetActorLocation() + FVector(4000, 4000, 0) * randomVal.VRand(), FRotator());
		checkf(enemy != nullptr, TEXT("ASideRoomsGameState::InitGame -> Failed to spawn enemy character"));
		EnemyCharacterRefList.Add(enemy);
		enemy->EnableMovement();
	}
	
}

void ASideRoomsGameState::TryUpdateMissionCount()
{
	CurrentMissionCount += 1;

	ASideRoomsGameMode* gamemode = Cast<ASideRoomsGameMode>(GetWorld()->GetAuthGameMode());
	if (gamemode)
	{
		gamemode->CheckMissionComplete(CurrentMissionCount);
	}
}