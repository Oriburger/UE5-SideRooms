// Copyright Epic Games, Inc. All Rights Reserved.

#include "../public/SideRoomsCharacter.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"


//////////////////////////////////////////////////////////////////////////
// ASideRoomsCharacter

ASideRoomsCharacter::ASideRoomsCharacter()
{
	//Tick Enable
	PrimaryActorTick.bCanEverTick = true;

	// Enable Crouch option
	GetCharacterMovement()->GetNavAgentPropertiesRef().bCanCrouch = true;

	// Initialize Character Stat
	CharacterHP = 3;
	SprintMultipleVal = 2.0f;

	CharacterStamina = 100;
	StaminaFlag = 0;
	StaminaIncreaseval = 0.1f;
	StaminaReduceVal = 0.2f;

	// Character doesnt have a rifle at start
	bHasRifle = false;
	
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(55.f, 96.0f);
		
	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1P = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh1P"));
	Mesh1P->SetOnlyOwnerSee(true);
	Mesh1P->SetupAttachment(FirstPersonCameraComponent);
	Mesh1P->bCastDynamicShadow = false;
	Mesh1P->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	Mesh1P->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

}

void ASideRoomsCharacter::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	// Verify that SideRoomsCharacter class is being used
	check(GEngine != nullptr);

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}

void ASideRoomsCharacter::Tick(float DeltaSeconds)
{
	// Call the base class
	Super::Tick(DeltaSeconds);

	if (0 <= CharacterStamina && CharacterStamina <= 100)
	{
		if (StaminaFlag == 1)
			CharacterStamina += StaminaIncreaseval;
		else
			CharacterStamina += StaminaFlag * StaminaReduceVal;
	}
	else
	{
		if (CharacterStamina <= 0)
			CharacterStamina = 0;
		else
			CharacterStamina = 100;
		StaminaFlag = 1;
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void ASideRoomsCharacter::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &ASideRoomsCharacter::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &ASideRoomsCharacter::Look);

		////Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &ASideRoomsCharacter::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &ASideRoomsCharacter::Sprinting);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &ASideRoomsCharacter::StopSprinting);

		//Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &ASideRoomsCharacter::Crouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &ASideRoomsCharacter::UnCrouch);
	}
}


void ASideRoomsCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add movement 
		AddMovementInput(GetActorForwardVector(), MovementVector.Y);
		AddMovementInput(GetActorRightVector(), MovementVector.X);
	}
}

void ASideRoomsCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	if (Controller != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(LookAxisVector.X);
		AddControllerPitchInput(LookAxisVector.Y);
	}
}

void ASideRoomsCharacter::Sprint(const FInputActionValue& Value)
{
	if (GetCharacterMovement()->GetCurrentAcceleration().IsNearlyZero()) return;
	if (bIsCrouched) return;

	GetCharacterMovement()->MaxWalkSpeed *= SprintMultipleVal;
	StaminaFlag = -1;
	FirstPersonCameraComponent->SetFieldOfView(100);
}

void ASideRoomsCharacter::Sprinting(const FInputActionValue& Value)
{
	if (CharacterStamina <= 0 || bIsCrouched || GetCharacterMovement()->GetCurrentAcceleration().IsNearlyZero())
	{
		StopSprinting(Value);
		return;
	}
}

void ASideRoomsCharacter::StopSprinting(const FInputActionValue& Value)
{
	if (StaminaFlag == -1)
	{
		GetCharacterMovement()->MaxWalkSpeed /= SprintMultipleVal;
		StaminaFlag = 1;
		FirstPersonCameraComponent->SetFieldOfView(90);
	}
}

void ASideRoomsCharacter::Crouch(const FInputActionValue& Value)
{
	Super::Crouch();
}

void ASideRoomsCharacter::UnCrouch(const FInputActionValue& Value)
{
	Super::UnCrouch();
}

void ASideRoomsCharacter::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool ASideRoomsCharacter::GetHasRifle()
{
	return bHasRifle;
}