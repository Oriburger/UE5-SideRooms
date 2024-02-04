// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "SideRoomsGameMode.generated.h"

USTRUCT(BlueprintType)
struct FDifficultyInfoStruct
{
public:
	GENERATED_USTRUCT_BODY()

	UPROPERTY(BlueprintReadOnly)
		bool bIsInvincibility = false;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (UIMin = 0.5f, UIMax = 1000.0f))
		float MaxHP = 100.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (UIMin = 100.0f, UIMax = 700.0f))
		float MoveSpeed = 400.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (UIMin = 0.0f, UIMax = 2.0f))
		float Defense = 0.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (UIMin = 100.0f, UIMax = 2000.0f))
		float SightRadius = 500.0f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, meta = (UIMin = 1000.0f, UIMax = 5000.0f))
		float HearingRadius = 1000.0f;
};

UCLASS(minimalapi)
class ASideRoomsGameMode : public AGameModeBase
{
	GENERATED_BODY()

//====== Basic Event ================
public:
	ASideRoomsGameMode();

	virtual void PreLogin(const FString& Options, const FString& Address, const FUniqueNetIdRepl& UniqueId, FString& ErrorMessage) override;

	virtual void PostLogin(APlayerController* NewPlayer) override;

protected:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void StartGame();

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
		void FinishGame();

private:
	UPROPERTY()
		TArray<TWeakObjectPtr<APlayerController> > PlayerControllerList;

	bool bHasBegun = false;

//====== Game Rule =================
private:
	int32 CurrentDifficulty = 0;

	FTimerHandle ElapsedTimer;

protected:
	//Interval value of updating difficulty
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay|Difficulty")
		float DifficultyUpdateInterval = 20.0f;

	//Info list of difficulty (mob's stat)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Gameplay|Difficulty")
		TArray<FDifficultyInfoStruct> DifficultyInfoList;

	//Increase difficulty using difficultyInfoList
	UFUNCTION()
		void UpdateDifficulty();

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		int32 GetCurrentDifficulty() { return CurrentDifficulty; }

//======= Utility ==============
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		FString GetMyIpAddress();
};



