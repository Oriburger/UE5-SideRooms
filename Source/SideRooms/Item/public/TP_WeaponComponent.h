// C	opyright Epic Games, Inc. All Rights Reserved.

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

//===== Attachment =============================
public:
	UPROPERTY()
		UStaticMeshComponent* ScopeComponent;

	UPROPERTY(BlueprintReadOnly)
		UStaticMeshComponent* MagazineComponent;

protected:
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Attachment")
		UStaticMesh* ScopeMesh;
	
	UPROPERTY(EditDefaultsOnly, Category = "Weapon|Attachment")
		UStaticMesh* MagazineMesh;

//===== Action =============================
public:
	/** Attaches the actor to a FirstPersonCharacter */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void AttachWeapon(AMainCharacterBase* TargetCharacter);

	/** Make the weapon Fire a Projectile */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void Fire();

	/** Reload */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void Reload();

	/** Aim */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void StartAiming();

	/** Aim */
	UFUNCTION(BlueprintCallable, Category = "Weapon")
		void StopAiming();

	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* WeaponMappingContext;

	/** Fire Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* FireAction;
	
	/** Reload Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* ReloadAction;

	/** Aim Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* AimAction;

protected:
	UFUNCTION(BlueprintImplementableEvent)
		void SimulateRecoil();

	UFUNCTION()
		TArray<FHitResult> GetBulletHitResult();

//===== Asset =============================
public:
	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Asset|Animation")
		UAnimMontage* FireAnimation;

	/** AnimMontage to play each time we fire */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Asset|Animation")
		UAnimMontage* CharacterFireAnimation;

	/** AnimMontage to play each time we reload */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Asset|Animation")
		UAnimMontage* ReloadAnimation;

	/** AnimMontage to play each time we reload */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Asset|Animation")
		UAnimMontage* CharacterReloadAnimation;
	
	/** AnimMontage to play each time we reload */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Asset|Animation")
		UAnimMontage* EmptyReloadAnimation;

	/** AnimMontage to play each time we reload */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Asset|Animation")
		UAnimMontage* CharacterEmptyReloadAnimation;

	/** Bullet splah effect  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Asset|VFX")
		UParticleSystem* BulletSplashEffect;

	/** Shoot Decal  */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Asset|VFX")
		UMaterialInterface* DefaultDecal;

	/** Sound Asset to play each time we reload */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Asset|Sound")
		USoundBase* ReloadSound;

	/** Sound Asset to play each time we reload */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay|Asset|Sound")
		USoundBase* EmptyReloadSound;

	/** Gun muzzle's offset from the characters location */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Gameplay")
		FVector MuzzleOffset;

private:
	UFUNCTION()
		float PlayAnimMontage(UAnimMontage* MeshAnimation, UAnimMontage* CharacterAnimation, float InPlayRate = 1.0f);

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

	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Gameplay|Stat")
		bool bInfiniteMagazine = false;

private:
	int32 CurrentClipSize;

	int32 CurrentAmmo;

	bool bIsAutomatic = false;

	bool bCanFire = true;
	
	bool bIsReloading = false;

	bool bIsAiming = false;

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

	UFUNCTION(BlueprintCallable, BlueprintPure)
		bool GetIsAiming() { return bIsAiming; }

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
