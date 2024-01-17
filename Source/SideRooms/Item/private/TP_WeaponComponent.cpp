// Copyright Epic Games, Inc. All Rights Reserved.


#include "../public/TP_WeaponComponent.h"
#include "../../Character/public/MainCharacterBase.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"

#define TRACE_LIMIT_DISTANCE 50000.0f

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);
}

void UTP_WeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentClipSize = MaxClipSize; 
}

void UTP_WeaponComponent::Fire()
{
	//Check Validity
	if (!CharacterRef.IsValid() || !IsValid(CharacterRef.Get()->GetController())) return;

	//Check Property Condition 
	if (!bCanFire || bIsReloading || CurrentClipSize <= 0) return;

	//Update Current ClipSize
	CurrentClipSize -= 1;

	//Hit Scan & Damage Logic
	TArray<FHitResult> targetList = GetBulletHitResult();
	for (FHitResult& target : targetList)
	{
		if (!IsValid(target.GetActor())) continue;
		UGameplayStatics::ApplyDamage(target.GetActor(), 1.0f, nullptr, CharacterRef.Get(), nullptr);
	}

	// Try and play the sound if specified
	if (FireSound != nullptr)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, CharacterRef.Get()->GetActorLocation());
	}

	// Try and play a firing animation if specified
	if (FireAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = CharacterRef.Get()->FirstPersonMesh->GetAnimInstance();
		if (AnimInstance != nullptr)
		{
			AnimInstance->Montage_Play(FireAnimation, 1.f);
		}
	}
	
	//Auto Fire / Reload Condition
	if (CurrentClipSize == 0)
	{
		Reload();
	}
	else
	{
		bCanFire = false;

		//Set Resetting bCanFire Event And  Auto Fire Event
		GetWorld()->GetTimerManager().SetTimer(AutoFireHandle, FTimerDelegate::CreateLambda([&]()
			{
				bCanFire = true;
				if (bIsAutomatic)
				{
					Fire();
				}
			}), FireRate, false);	// 반복하려면 false를 true로 변경
	}
}

void UTP_WeaponComponent::Reload()
{
	//Check Property Condition 
	if (bIsReloading || CurrentAmmo <= 0) return;

	//Set Property Condition
	bIsReloading = true; 
	bCanFire = false;

	//ReloadAnimation 
	//~

	//ReloadEffect 
	//~
	
	GetWorld()->GetTimerManager().SetTimer(ReloadDelayHandle, FTimerDelegate::CreateLambda([&]()
		{
			//Update Ammo Info
			const int32 needAmmo = MaxClipSize - CurrentClipSize;
			const int32 addAmmo = FMath::Min(needAmmo, CurrentAmmo);
			CurrentAmmo -= addAmmo;
			CurrentClipSize += addAmmo;

			//Update Property Condition
			bCanFire = true;
			bIsReloading = false;

		}), ReloadSpeed, false);	// 반복하려면 false를 true로 변경
}

TArray<FHitResult> UTP_WeaponComponent::GetBulletHitResult()
{
	if (GetSkinnedAsset()->FindSocket(FName("Muzzle")) == nullptr) return {};
	if (!CharacterRef.IsValid() || !IsValid(GetOwner())) return {};

	//try trace by multi channel muzzle location to camera inf location 
	const FVector& beginLocation = GetSocketLocation(FName("Muzzle"));
	const FVector& endLocation = (CharacterRef.Get()->FirstPersonCameraComponent->GetComponentLocation())
		+ CharacterRef.Get()->FirstPersonCameraComponent->GetComponentRotation().Vector() * TRACE_LIMIT_DISTANCE;
	
	//only trace on world dynamic, pawn
	TArray<FHitResult> hitResult;
	UKismetSystemLibrary::LineTraceMultiForObjects(GetWorld(), beginLocation, endLocation
		, { (EObjectTypeQuery)ECC_WorldDynamic, (EObjectTypeQuery)ECC_Pawn }, false, {GetOwner()}
		, EDrawDebugTrace::Type::ForDuration, hitResult, true);
	return hitResult;
}

void UTP_WeaponComponent::AttachWeapon(AMainCharacterBase* TargetCharacter)
{
	CharacterRef = TargetCharacter;
	if (!CharacterRef.IsValid()) return;

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(CharacterRef.Get()->FirstPersonMesh, AttachmentRules, FName(TEXT("GripPoint")));

	// switch bHasRifle so the animation blueprint can switch to another animation set
	CharacterRef.Get()->SetHasRifle(true);
	if (CharacterRef.Get()->GetClass() == UTP_WeaponComponent::StaticClass())
	{
		Cast<AMainCharacterBase>(CharacterRef.Get())->WeaponRef = this;
	}

	// Set up action bindings
	if (APlayerController* PlayerController = Cast<APlayerController>(CharacterRef.Get()->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			// Set the priority of the mapping to 1, so that it overrides the Jump action with the Fire action when using touch input
			Subsystem->AddMappingContext(WeaponMappingContext, 1);
		}

		if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerController->InputComponent))
		{
			// Fire
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Fire);

			// Reload
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Reload);
		}
	}
}

void UTP_WeaponComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	if (!CharacterRef.IsValid()) return;

	if (APlayerController* PlayerController = Cast<APlayerController>(CharacterRef.Get()->GetController()))
	{
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PlayerController->GetLocalPlayer()))
		{
			Subsystem->RemoveMappingContext(WeaponMappingContext);
		}
	}
}