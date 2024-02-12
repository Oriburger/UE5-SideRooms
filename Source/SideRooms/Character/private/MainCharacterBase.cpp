// Copyright Epic Games, Inc. All Rights Reserved.

#include "../public/MainCharacterBase.h"
#include "../public/SideRoomPlayerController.h"
#include "Animation/AnimInstance.h"
#include "../../Item/public/Interactable.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "Kismet/KismetSystemLibrary.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/Controller.h"
#include "GameFramework/CharacterMovementComponent.h"

#define MAX_INTERACT_DISTANCE 300.0f;

//////////////////////////////////////////////////////////////////////////
// AMainCharacterBase

AMainCharacterBase::AMainCharacterBase()
{
	Tags.Add("Player");
	SetActorTickInterval(0.25f);

	// Initialize Character Stamina State
	StaminaIncreaseval = 0.1f;
	StaminaReduceVal = 0.2f;
	StaminaRate = 0.01f;

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

	CharacterCurrentStamina = CharacterMaxStamina;

	//Add Input Mapping Context
	if (APlayerController* PlayerController = Cast<APlayerController>(Controller))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->AddMappingContext(DefaultMappingContext, 0);
		}
	}

	//Init Default FOV
	CurrentFieldOfView = FirstPersonCameraComponent->FieldOfView;
}

void AMainCharacterBase::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	//Find Interactive Prop using Tick per 0.25 sec
	FindInteractiveProp();
}

float AMainCharacterBase::TakeDamage(float Damage, FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
{
	return Super::TakeDamage(Damage, DamageEvent, EventInstigator, DamageCauser);
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

		//Sprinting
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AMainCharacterBase::Sprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Triggered, this, &AMainCharacterBase::CheckStopSprint);
		EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AMainCharacterBase::StopSprint);
		
		//Crouching
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Triggered, this, &AMainCharacterBase::StartCrouch);
		EnhancedInputComponent->BindAction(CrouchAction, ETriggerEvent::Completed, this, &AMainCharacterBase::StopCrouch);
	
		//Interacting
		EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Triggered, this, &AMainCharacterBase::Interact);
	}
}

void AMainCharacterBase::FindInteractiveProp()
{
	if (!bCanInteract) return; 
	FVector beginLocation = FirstPersonCameraComponent->GetComponentLocation();
	FVector endLocation = beginLocation + FirstPersonCameraComponent->GetForwardVector() * MAX_INTERACT_DISTANCE;

	FHitResult hitResult;
	UKismetSystemLibrary::LineTraceSingle(GetWorld(), beginLocation, endLocation
										, (ETraceTypeQuery)ECC_GameTraceChannel2, false, {this}
										, EDrawDebugTrace::None, hitResult, true, FColor::Red, FColor::Green, 0.3f);
	
	if (hitResult.bBlockingHit)
	{
		if (hitResult.GetComponent()->GetClass()->ImplementsInterface(UInteractable::StaticClass()))
		{
			InteractablePropRef = hitResult.GetComponent();
			InteractablePropRef->Activate();
			return;
		}
	}	
	if(InteractablePropRef != nullptr)
	{
		InteractablePropRef->Deactivate();
		InteractablePropRef = nullptr;
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
	if (GetIsSprinting()) return;

	StaminaFlag = -1;
	SetNewFieldOfView(100.0f);
	if (!GetWorld()->GetTimerManager().IsTimerActive(SprintHandle))
		GetWorld()->GetTimerManager().SetTimer(SprintHandle, this, &AMainCharacterBase::StaminaFunction, StaminaRate, true);
}

void AMainCharacterBase::CheckStopSprint(const FInputActionValue& Value)
{
	if (CharacterCurrentStamina <= 0 || bIsCrouched || GetCharacterMovement()->GetCurrentAcceleration().IsNearlyZero())
	{
		StopSprint(Value);
		return;
	}
}

void AMainCharacterBase::StopSprint(const FInputActionValue& Value)
{
	StaminaFlag = 1;
	Super::StopSprint(Value);
	SetNewFieldOfView(90);
}

void AMainCharacterBase::StartCrouch(const FInputActionValue& Value)
{
	Super::Crouch();
}

void AMainCharacterBase::StopCrouch(const FInputActionValue& Value)
{
	Super::UnCrouch();
}

void AMainCharacterBase::Interact(const FInputActionValue& Value)
{
	if(InteractablePropRef == nullptr) return;
	InteractablePropRef->Interact(this);
}

void AMainCharacterBase::Die()
{
	Super::Die();
}

void AMainCharacterBase::EnableMovement()
{
	Super::EnableMovement();
}

void AMainCharacterBase::DisableMovement()
{
	Super::DisableMovement();
}


void AMainCharacterBase::SetNewFieldOfView(float Value)
{
	TargetFieldOfView = Value;
	GetWorldTimerManager().ClearTimer(FOVInterpHandle);
	GetWorldTimerManager().SetTimer(FOVInterpHandle, this, &AMainCharacterBase::UpdateFOVValue, 0.01f, true);
}

void AMainCharacterBase::UpdateFOVValue()
{
	if (FMath::IsNearlyEqual(CurrentFieldOfView, TargetFieldOfView, 0.1))
		GetWorldTimerManager().ClearTimer(FOVInterpHandle);
	CurrentFieldOfView = FMath::FInterpTo(CurrentFieldOfView, TargetFieldOfView, 0.1f, 1.0f);
	FirstPersonCameraComponent->SetFieldOfView(CurrentFieldOfView);
}

void AMainCharacterBase::SetHasRifle(bool bNewHasRifle)
{
	bHasRifle = bNewHasRifle;
}

bool AMainCharacterBase::GetHasRifle()
{
	return bHasRifle;
}

void AMainCharacterBase::StaminaFunction()
{
	if (0 <= CharacterCurrentStamina && CharacterCurrentStamina <= 100)
	{
		if (StaminaFlag == 1)
			CharacterCurrentStamina += StaminaIncreaseval;
		else if (StaminaFlag == -1)
			CharacterCurrentStamina -= StaminaReduceVal;
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
			GetWorld()->GetTimerManager().ClearTimer(SprintHandle);
		}
	}
}