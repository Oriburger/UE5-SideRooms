// Copyright Epic Games, Inc. All Rights Reserved.

#include "../public/MainCharacterBase.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/CharacterMovementComponent.h"


//////////////////////////////////////////////////////////////////////////
// AMainCharacterBase

AMainCharacterBase::AMainCharacterBase()
{
	// Initialize Character Stamina State
	CharacterCurrentStamina = CharacterMaxStamina;
	StaminaFlag = 0;
	StaminaIncreaseval = 0.1f;
	StaminaReduceVal = 0.2f;

	// Character doesnt have a rifle at start
	bHasRifle = false;

	// Create a CameraComponent	
	FirstPersonCameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	FirstPersonCameraComponent->SetupAttachment(GetCapsuleComponent());
	FirstPersonCameraComponent->SetRelativeLocation(FVector(-10.f, 0.f, 60.f)); // Position the camera
	FirstPersonCameraComponent->bUsePawnControlRotation = true;

	// Create a Spring arm component
	SpringArmComponent = CreateDefaultSubobject<USpringArmComponent>(TEXT("SpringArm"));
	SpringArmComponent->SetupAttachment(FirstPersonCameraComponent);

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	FirstPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	FirstPersonMesh->SetOnlyOwnerSee(true);
	FirstPersonMesh->SetupAttachment(SpringArmComponent);
	FirstPersonMesh->bCastDynamicShadow = false;
	FirstPersonMesh->CastShadow = false;
	//Mesh1P->SetRelativeRotation(FRotator(0.9f, -19.19f, 5.2f));
	FirstPersonMesh->SetRelativeLocation(FVector(-30.f, 0.f, -150.f));

	GetCharacterMovement()->NavAgentProps.bCanCrouch = true;
}

void AMainCharacterBase::BeginPlay()
{
	// Call the base class  
	Super::BeginPlay();

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}
}


void AMainCharacterBase::Tick(float DeltaSeconds)
{
	// Call the base class
	Super::Tick(DeltaSeconds);

	UE_LOG(LogTemp, Warning, TEXT("Tick#1 %d"));

	if (0 <= CharacterCurrentStamina && CharacterCurrentStamina <= CharacterMaxStamina)
	{
		UE_LOG(LogTemp, Warning, TEXT("Tick#2"));
		if (StaminaFlag == 1)
			CharacterCurrentStamina += StaminaIncreaseval;
		else
			CharacterCurrentStamina += StaminaFlag * StaminaReduceVal;
	}
	else
	{
		if (CharacterCurrentStamina <= 0)
		{
			CharacterCurrentStamina = 0;
			StaminaFlag = 1;
		}
		else
		{
			CharacterCurrentStamina = 100;
			StaminaFlag = 0;
		}
	}
}

//////////////////////////////////////////////////////////////////////////// Input

void AMainCharacterBase::SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent)
{
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = CastChecked<UEnhancedInputComponent>(PlayerInputComponent))
	{
		//Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Triggered, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		//Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AMainCharacterBase::Move);

		//Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AMainCharacterBase::Look);

		////Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AMainCharacterBase::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AMainCharacterBase::CheckStopSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMainCharacterBase::StopSprint);
		
		//Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AMainCharacterBase::StartCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AMainCharacterBase::StopCrouch);
	}
}

void AMainCharacterBase::Move(const FInputActionValue& Value)
{
	Super::Move(Value);
}

void AMainCharacterBase::Look(const FInputActionValue& Value)
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

void AMainCharacterBase::Sprint(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Sprint#1"));
	if (GetIsSprinting()) return;
	UE_LOG(LogTemp, Warning, TEXT("Sprint#2"));
	Super::Sprint(Value);
	StaminaFlag = -1;
	FirstPersonCameraComponent->SetFieldOfView(100);
}

void AMainCharacterBase::CheckStopSprint(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Check Stop Sprint #1"));
	if (CharacterCurrentStamina <= 0 || bIsCrouched || GetCharacterMovement()->GetCurrentAcceleration().IsNearlyZero())
	{
		UE_LOG(LogTemp, Warning, TEXT("Check Stop Sprint #2"));
		StopSprint(Value);
		return;
	}
}

void AMainCharacterBase::StopSprint(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Stop Sprint #1"));
	if (StaminaFlag == -1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Stop Sprint #2"));
		Super::StopSprint(Value);
		StaminaFlag = 1;
		FirstPersonCameraComponent->SetFieldOfView(90);
	}
}

void AMainCharacterBase::StartCrouch(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Crouch"));
	Super::Crouch();
}

void AMainCharacterBase::StopCrouch(const FInputActionValue& Value)
{
	UE_LOG(LogTemp, Warning, TEXT("Stop Crouch"));
	Super::UnCrouch();
}

void AMainCharacterBase::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AMainCharacterBase::GetHasRifle()
{
	return bHasRifle;
}