// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "InputActionValue.h"
#include "SideRoomsCharacter.generated.h"

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

class UInputComponent;
class USkeletalMeshComponent;
class USceneComponent;
class UCameraComponent;
class UAnimMontage;
class USoundBase;

UCLASS(Blueprintable, config=Game)
class ASideRoomsCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	ASideRoomsCharacter();

protected:
	virtual void BeginPlay();
	virtual void Tick(float DeltaSeconds) override;

//======== Component ============================
public:
	/** Pawn mesh: 1st person view (arms; seen only by self) */
	UPROPERTY(VisibleDefaultsOnly, Category = Mesh)
		USkeletalMeshComponent* Mesh1P;

	/** First person camera */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Camera, meta = (AllowPrivateAccess = "true"))
		UCameraComponent* FirstPersonCameraComponent;

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

	/** Sprint Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* SprintAction;

	/** Crouch Input Action */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Input, meta = (AllowPrivateAccess = "true"))
		class UInputAction* CrouchAction;

protected:
	/** Called for movement input */
	void Move(const FInputActionValue& Value);

	/** Called for looking input */
	void Look(const FInputActionValue& Value);

	/** Called for starting sprint input */
	void Sprint(const FInputActionValue& Value);

	/** Called for sprinting input */
	void Sprinting(const FInputActionValue& Value);

	/** Called for stop sprinting input */
	void StopSprinting(const FInputActionValue& Value);

	/** Called for crouching input */
	void Crouch(const FInputActionValue& Value);

	/** Called for stop sprinting input */
	void UnCrouch(const FInputActionValue& Value);

protected:
	// APawn interface
	virtual void SetupPlayerInputComponent(UInputComponent* InputComponent) override;
	// End of APawn interface

//======= Stats ====================================
public:
	/** Character HP **/
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Stats)
		int32 CharacterHP;

	/** Character Stamina **/
	UPROPERTY(VisibleAnyWhere, BlueprintReadWrite, Category = Stats)
		float CharacterStamina;

	/** Character Sprint speed */
	UPROPERTY(EditAnywhere, Category = Stats)
		float SprintMultipleVal;

private:
	/** Flag for Stamina Control */
	int StaminaFlag;

	/**Stamina Recovery Value */
	float StaminaIncreaseval;

	/**Stamina Consuming Value */
	float StaminaReduceVal;

//======= Pickup ====================================
public:
	/** Bool for AnimBP to switch to another animation set */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = Weapon)
		bool bHasRifle;

	/** Setter to set the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
		void SetHasRifle(bool bNewHasRifle);

	/** Getter for the bool */
	UFUNCTION(BlueprintCallable, Category = Weapon)
		bool GetHasRifle();
};

