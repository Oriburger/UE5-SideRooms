// Copyright Epic Games, Inc. All Rights Reserved.


#include "../public/TP_WeaponComponent.h"
#include "../../Character/public/MainCharacterBase.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "Kismet/KismetMathLibrary.h"

#define TRACE_LIMIT_DISTANCE 50000.0f

// Sets default values for this component's properties
UTP_WeaponComponent::UTP_WeaponComponent()
{
	// Default offset from the character location for projectiles to spawn
	MuzzleOffset = FVector(100.0f, 0.0f, 10.0f);

	ScopeComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Scope"));
	ScopeComponent->SetupAttachment(this, "SOCKET_Default");
	ScopeComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	MagazineComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Magazine"));
	MagazineComponent->SetupAttachment(this, "SOCKET_Magazine");
	MagazineComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
}

void UTP_WeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentClipSize = MaxClipSize; 
	if (ScopeComponent != nullptr && MagazineComponent != nullptr)
	{
		ScopeComponent->SetStaticMesh(ScopeMesh);
		MagazineComponent->SetStaticMesh(MagazineMesh);

		ScopeComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform, TEXT("SOCKET_Default"));
		MagazineComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform, TEXT("SOCKET_Magazine"));
	}
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
		if (DefaultDecal != nullptr)
			UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DefaultDecal, FVector(10.0f), target.Location
												, UKismetMathLibrary::FindLookAtRotation( GetComponentLocation(), target.Location), 0.0f);
		if (IsValid(target.GetActor()))
			UGameplayStatics::ApplyDamage(target.GetActor(), 1.0f, nullptr, CharacterRef.Get(), nullptr);
	}

	// Simulate Recoil
	SimulateRecoil();

	// Try and play a firing animation if specified
	PlayAnimMontage(FireAnimation, CharacterFireAnimation);
	
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
	if (bIsReloading || (!bInfiniteMagazine && CurrentAmmo <= 0)) return;

	StopAiming();

	//Set Property Condition
	bIsReloading = true; 
	bCanFire = false;

	//ReloadAnimation & Sound
	if (CurrentClipSize == 0)
	{
		ReloadSpeed = PlayAnimMontage(EmptyReloadAnimation, CharacterEmptyReloadAnimation);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), EmptyReloadSound, GetComponentLocation());
	}
	else
	{
		ReloadSpeed = PlayAnimMontage(ReloadAnimation, CharacterReloadAnimation);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), ReloadSound, GetComponentLocation());
	}
	
	//ReloadEffect 
	//~
	
	GetWorld()->GetTimerManager().SetTimer(ReloadDelayHandle, FTimerDelegate::CreateLambda([&]()
		{
			//Update Ammo Info
			const int32 needAmmo = MaxClipSize - CurrentClipSize;
			const int32 addAmmo = FMath::Min(needAmmo, (int32)(bInfiniteMagazine ? 1e9 : CurrentAmmo));

			if(!bInfiniteMagazine) CurrentAmmo -= addAmmo;
			CurrentClipSize += addAmmo;

			//Update Property Condition
			bCanFire = true;
			bIsReloading = false;

		}), ReloadSpeed, false);	// 반복하려면 false를 true로 변경
}

void UTP_WeaponComponent::StartAiming()
{
	if (!CharacterRef.IsValid() || !IsValid(GetOwner())) return;
	bIsAiming = true;
	return;
}

void UTP_WeaponComponent::StopAiming()
{
	if (!CharacterRef.IsValid() || !IsValid(GetOwner())) return;
	bIsAiming = false;
	return;
}

TArray<FHitResult> UTP_WeaponComponent::GetBulletHitResult()
{
	if (!CharacterRef.IsValid() || !IsValid(GetOwner())) return {};

	//try trace by multi channel muzzle location to camera inf location 
	const FVector& beginLocation = GetSocketLocation(FName("SOCKET_Muzzle"));
	const FVector& endLocation = (CharacterRef.Get()->FirstPersonCameraComponent->GetComponentLocation())
		+ CharacterRef.Get()->FirstPersonCameraComponent->GetComponentRotation().Vector() * TRACE_LIMIT_DISTANCE;
	
	//only trace on world dynamic, pawn
	TArray<FHitResult> hitResult;
	UKismetSystemLibrary::LineTraceMultiForObjects(GetWorld(), beginLocation, endLocation
		, { (EObjectTypeQuery)ECC_WorldStatic, (EObjectTypeQuery)ECC_WorldDynamic, (EObjectTypeQuery)ECC_Pawn }, false
		, {GetOwner()}, EDrawDebugTrace::Type::ForDuration, hitResult, true);
	return hitResult;
}

float UTP_WeaponComponent::PlayAnimMontage(UAnimMontage* MeshAnimation, UAnimMontage* CharacterAnimation, float InPlayRate)
{
	float avgPlayTime = 0.0f;

	// Try and play a firing animation if specified
	if (MeshAnimation != nullptr)
	{
		UAnimInstance* meshAnimInstance = GetAnimInstance();
		if (meshAnimInstance != nullptr)
			avgPlayTime += meshAnimInstance->Montage_Play(MeshAnimation, InPlayRate);
	}

	if (CharacterAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* characterAnimInstance = CharacterRef.Get()->FirstPersonMesh->GetAnimInstance();
		if (characterAnimInstance != nullptr)
			avgPlayTime += characterAnimInstance->Montage_Play(CharacterAnimation, InPlayRate);
	}
	return avgPlayTime / 2.0f;
}

void UTP_WeaponComponent::AttachWeapon(AMainCharacterBase* TargetCharacter)
{
	CharacterRef = TargetCharacter;
	if (!CharacterRef.IsValid()) return;

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(CharacterRef.Get()->FirstPersonMesh, AttachmentRules, FName(TEXT("SOCKET_Weapon")));

	// switch bHasRifle so the animation blueprint can switch to another animation set
	CharacterRef.Get()->SetHasRifle(true);
	CharacterRef.Get()->WeaponRef = this;

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

			// Start Aiming
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &UTP_WeaponComponent::StartAiming);

			// Stop Aiming
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Canceled, this, &UTP_WeaponComponent::StopAiming);
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