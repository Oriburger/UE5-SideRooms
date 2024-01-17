// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "CharacterBase.h"
#include "InputActionValue.h"
#include "MainCharacterBase.generated.h"

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

UENUM(BlueprintType)
enum class ECharacterState : uint8
{
	E_IDLE		UMETA(DisplayName = "Idle"),
	E_WALK		UMETA(DisplayName = "Walk"),
	E_ATTACK	UMETA(DisplayName = "Attack"),
	E_SPRINT	UMETA(DisplayName = "Sprint"),
	E_JUMP		UMETA(DisplayName = "Jump"),
	E_CROUCH	UMETA(DisplayName = "Crouch"),
	E_HIT		UMETA(DisplayName = "Hit"),
	E_DEAD		UMETA(DisplayName = "Dead"),
};


UCLASS(config=Game)
class AMainCharacterBase : public ACharacterBase
{
	GENERATED_BODY()

public:
	AMainCharacterBase();

protected:
	virtual void BeginPlay();

	virtual void Tick(float DeltaSeconds) override;

//======== Component S
public:
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Mesh)
		USkeletalMeshComponent* FirstPersonMesh;
	
	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FirstPersonCameraComponent;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = Camera)
		class USpringArmComponent* SpringArmComponent;

//======== Input / Action ============================
public:
	/** MappingContext */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputMappingContext* DefaultMappingContext;

	/** Jump Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* JumpAction;

	/** Move Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* MoveAction;

	/** Look Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* LookAction;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* SprintAction;

	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* CrouchAction;

protected:
	/** Called for movement input */
	virtual void Move(const FInputActionValue& Value) override;

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for starting sprint input */
	void Sprint(const FInputActionValue& Value);

	/** Called for sprinting input */
	void Sprinting(const FInputActionValue& Value);

	/** Called for stop sprinting input */
	void StopSprinting(const FInputActionValue& Value);

	/** Called for crouching input */
	void StartCrouch(const FInputActionValue& Value);
	
	/** Called for stop sprinting input */
	void StopCrouch(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

//======= Stats ====================================
protected:
	/** Character MaxStamina */
	UPROPERTY(EditAnywhere, Category = "Gameplay|Stat")
		float CharacterMaxStamina;

	/** Character Sprint speed */
	UPROPERTY(EditAnywhere, Category = "Gameplay|Stat")
		float SprintMultipleVal;

private:
	/** Character Stamina **/
	float CharacterCurrentStamina;

	/** Flag for Stamina Control */
	int StaminaFlag;

	/**Stamina Recovery Value */
	float StaminaIncreaseval;

	/**Stamina Consuming Value */
	float StaminaReduceVal;

public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
		float GetCurrentStamina() { return CharacterCurrentStamina; }
		
	UFUNCTION(BlueprintCallable, BlueprintPure)
		float GetMaxStamina() { return CharacterMaxStamina; }

//======= Pickup ====================================
private:
	/** Bool for AnimBP to switch to another animation set */
	bool bHasRifle;

public:
	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
		void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = Weapon)
		bool GetHasRifle();
	
	//Temp Value
	UPROPERTY(BlueprintReadOnly)
		class UTP_WeaponComponent* WeaponRef;
};

