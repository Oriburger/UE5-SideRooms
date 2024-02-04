// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameState.h"
#include "SideRoomsGameState.generated.h"

/**
 * 
 */
UCLASS()
class SIDEROOMS_API ASideRoomsGameState : public AGameStateBase
{
	GENERATED_BODY()

//====== Game Start ================
public:
	UFUNCTION(BlueprintNativeEvent)
		void InitGame();

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
		TSubclassOf<class AEnemyCharacterBase> EnemyCharacterClass;

private:
	UFUNCTION()
		void InitMap();

	UFUNCTION()
		void InitMonster();

	UFUNCTION()
		void InitPlayer();

//====== Game Property =====================
private:
	UPROPERTY()
		TArray<TWeakObjectPtr<AEnemyCharacterBase> > EnemyCharacterRefList;

	//UPROPERTY()
	//	TArray<class AMissionPropBase> MissionPropRefList;

//====== Mission State / Update =================
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		int32 GetCurrentMissionCount() { return CurrentMissionCount; }

protected:
	UFUNCTION(BlueprintCallable)
		void TryUpdateMissionCount(int32 AddValue);

private:
	int32 CurrentMissionCount = 0;

	UFUNCTION(NetMulticast, Reliable)
		void UpdateMissionCount(int32 AddValue);
	
	UFUNCTION(Server, Reliable)
		void ServerRPCUpdateMissionCount(int32 AddValue);


};