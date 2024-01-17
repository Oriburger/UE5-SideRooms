// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "Components/SkeletalMeshComponent.h"
#include "TP_WeaponComponent.generated.h"

class AMainCharacterBase;

UCLASS(Blueprintable, BlueprintType, ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class SIDEROOMS_API UTP_WeaponComponent : public USkeletalMeshComponent
{
	GENERATED_BODY()

//===== Basic =============================
public:
	/** Sets default values for this component's properties */
	UTP_WeaponComponent();

protected:
	UFUNCTION()
		virtual void BeginPlay() override;

	/** Ends gameplay for this component. */
	UFUNCTION()
		virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

//===== Action =============================
public:
	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void AttachWeapon(AMainCharacterBase* TargetCharacter);

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void Fire();

	/** Make the weapon  */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void Reload();

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* WeaponMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* FireAction;
	
	/** Reload Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* ReloadAction;

protected:
	UFUNCTION()
		TArray<FHitResult> GetBulletHitResult();

//===== Asset =============================
public:
	/** Sound to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category=Gameplay)
		USoundBase* FireSound;
	
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		UAnimMontage* FireAnimation;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Gameplay)
		FVector MuzzleOffset;

//===== Stat / State =============================
protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gameplay|Stat")
		int32 MaxClipSize;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gameplay|Stat")
		int32 MaxAmmoCount;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gameplay|Stat")
		float FireRate;

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gameplay|Stat")
		float ReloadSpeed;

private:
	int32 CurrentClipSize;

	int32 CurrentAmmo;

	bool bIsAutomatic = false;

	bool bCanFire = true;
	
	bool bIsReloading = false;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		int32 GetCurrentClipSize() { return CurrentClipSize; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		int32 GetMaxClipSize() { return MaxClipSize; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		int32 GetCurrentAmmo() { return CurrentAmmo; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		int32 GetMaxAmmo() { return MaxAmmoCount; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool GetIsAutomatic() { return bIsAutomatic; }

	UFUNCTION(BlueprintCallable)
		void SetIsAutomatic(bool NewState) { bIsAutomatic = NewState; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool GetCanFire() { return bCanFire; }

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool GetIsReloading() { return bIsReloading; }

//===== Etc =============================
private:
	/** The Character holding this weapon*/
	TWeakObjectPtr<AMainCharacterBase> CharacterRef;

protected:	
	UFUNCTION(BlueprintCallable, BlueprintPure)
		AMainCharacterBase* GetCharacterRef() { return CharacterRef.Get(); }

	UPROPERTY(BlueprintReadOnly)
		FTimerHandle AutoFireHandle;
	
	UPROPERTY(BlueprintReadOnly)
		FTimerHandle ReloadDelayHandle;

};
