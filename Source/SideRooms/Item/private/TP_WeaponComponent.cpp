// Copyright Epic Games, Inc. All Rights Reserved.


#include "../public/TP_WeaponComponent.h"
#include "../../Character/public/MainCharacterBase.h"
#include "Engine/Classes/Particles/ParticleSystemComponent.h"
#include "GameFramework/PlayerController.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/CameraComponent.h"
#include "Kismet/GameplayStatics.h"	
#include "Perception/AISense_Hearing.h"
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
	MagazineComponent = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Magazine"));
}

void UTP_WeaponComponent::BeginPlay()
{
	Super::BeginPlay();

	CurrentClipSize = MaxClipSize; 
	if (ScopeMesh != nullptr && MagazineMesh != nullptr && FireFlashEmitter)
	{	
		ScopeComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		MagazineComponent->SetCollisionEnabled(ECollisionEnabled::NoCollision);

		ScopeComponent->SetStaticMesh(ScopeMesh);
		MagazineComponent->SetStaticMesh(MagazineMesh);

		ScopeComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform, TEXT("SOCKET_Default"));
		MagazineComponent->AttachToComponent(this, FAttachmentTransformRules::KeepRelativeTransform, TEXT("SOCKET_Magazine"));
	}
}

void UTP_WeaponComponent::Fire_Implementation()
{	
	//Check Validity
	if (!CharacterRef.IsValid() || !IsValid(CharacterRef.Get()->GetController())) return;

	//Check Property Condition 
	if (!bCanFire || bIsReloading || CurrentClipSize <= 0) return;

	//Update Current ClipSize
	CurrentClipSize -= 1;

	//Hit Scan & Damage Logic
	TArray<FHitResult> targetList = GetBulletHitResult();
	TArray<AActor*> damageActorList;

	for (FHitResult& target : targetList)
	{
		FString str = UKismetSystemLibrary::GetDisplayName(target.GetComponent());
		UE_LOG(LogTemp, Warning, TEXT("Target : %s"), *str);
		if (IsValid(target.GetActor()) && target.GetActor()->ActorHasTag("Enemy"))
		{
			float damageAmount = 1.0f;

			if (target.GetComponent()->ComponentHasTag("Head"))
			{
				damageAmount = 2.0f;
				//Head Hit Event
				// ...
			}
			else if (target.GetComponent()->ComponentHasTag("LeftLeg"))
			{
				damageAmount = 1.5f;
				//Leg Hit Event
				// ...
			}
			else if (target.GetComponent()->ComponentHasTag("RightLeg"))
			{
				damageAmount = 1.5f;
				//Leg Hit Event
				// ...
			}

			UGameplayStatics::ApplyDamage(target.GetActor(), damageAmount, nullptr, CharacterRef.Get(), nullptr);
		}
		if (DefaultDecal != nullptr && target.GetComponent() != nullptr
			&& target.GetComponent()->GetCollisionObjectType() != ECollisionChannel::ECC_Pawn)
		{
			UGameplayStatics::SpawnDecalAtLocation(GetWorld(), DefaultDecal, FVector(10.0f), target.Location
				, UKismetMathLibrary::FindLookAtRotation(GetComponentLocation(), target.Location), 0.0f);
		}
	}

	//Play sound effect with perception trigger
	TrySpawnFireEffect();
	UAISense_Hearing::ReportNoiseEvent(GetWorld(), GetComponentLocation(), 10.0f, CharacterRef.Get());
	CharacterRef.Get()->MakeNoise(10.0f, CharacterRef.Get(), GetComponentLocation());

	// Simulate Recoil
	SimulateRecoil();

	// Try and play a firing animation if specified
	TryPlayAnimMontage(FireAnimation, CharacterFireAnimation);
	
	//Auto Fire / Reload Condition
	if (CurrentClipSize == 0)
	{
		GetWorld()->GetTimerManager().SetTimer(AutoFireHandle, FTimerDelegate::CreateLambda([&]() { Reload(); }), 1.0f, false, 0.25f);
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
		ReloadSpeed = 1.0f;
		TryPlayAnimMontage(EmptyReloadAnimation, CharacterEmptyReloadAnimation);
		UGameplayStatics::PlaySoundAtLocation(GetWorld(), EmptyReloadSound, GetComponentLocation());
	}
	else
	{
		ReloadSpeed = 1.0f;
		TryPlayAnimMontage(ReloadAnimation, CharacterReloadAnimation);
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

		}), 1.0f, false, 2.5f);	// 반복하려면 false를 true로 변경
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

void UTP_WeaponComponent::TryFire()
{
	if (!GetOwner()->HasAuthority())
	{
		ServerRPCFire(); 
	}
	else
	{
		Fire();
	}
}

void UTP_WeaponComponent::ServerRPCFire_Implementation()
{
	Fire();
}

TArray<FHitResult> UTP_WeaponComponent::GetBulletHitResult()
{
	if (!CharacterRef.IsValid() || !IsValid(GetOwner())) return {};

	//try trace by multi channel muzzle location to camera inf location 
	const FVector& beginLocation = GetSocketLocation(FName("SOCKET_Muzzle"));
	const FVector& endLocation = (CharacterRef.Get()->FirstPersonCameraComponent->GetComponentLocation())
		+ CharacterRef.Get()->FirstPersonCameraComponent->GetComponentRotation().Vector() * TRACE_LIMIT_DISTANCE;

	FCollisionQueryParams collisionQueryParams;
	collisionQueryParams.AddIgnoredActor(CharacterRef.Get());
	collisionQueryParams.AddIgnoredComponent(this);

	//only trace on world dynamic, pawn
	FHitResult hitResult;
	GetWorld()->LineTraceSingleByChannel(hitResult, beginLocation, endLocation
										, ECollisionChannel::ECC_Camera, collisionQueryParams);
	
	if (hitResult.bBlockingHit)
	{
		DrawDebugLine(GetWorld(), hitResult.Location, endLocation, FColor::Green, false, 2.0f, 0, 1.5f);
		DrawDebugLine(GetWorld(), beginLocation, hitResult.Location, FColor::Red, false, 2.0f, 0, 2.0f);
	}

	return { hitResult };
}

void UTP_WeaponComponent::PlayAnimMontage_Implementation(UAnimMontage* MeshAnimation, UAnimMontage* CharacterAnimation, float InPlayRate)
{
	float avgPlayTime = 0.0f;

	// Try and play a firing animation if specified
	if (MeshAnimation != nullptr)
	{
		UAnimInstance* meshAnimInstance = GetAnimInstance();
		if (meshAnimInstance != nullptr)
			meshAnimInstance->Montage_Play(MeshAnimation, InPlayRate);
	}

	if (CharacterAnimation != nullptr)
	{
		// Get the animation object for the arms mesh
		if (CharacterRef.Get()->FirstPersonMesh)
		{
			UAnimInstance* characterAnimInstance = CharacterRef.Get()->FirstPersonMesh->GetAnimInstance();
			if (characterAnimInstance != nullptr)
				characterAnimInstance->Montage_Play(CharacterAnimation, InPlayRate);
		}
	}
}

void UTP_WeaponComponent::ServerRPCPlayAnimMontage_Implementation(UAnimMontage* AnimMontage, UAnimMontage* CharacterMontage, float InPlayRate)
{
	PlayAnimMontage(AnimMontage, CharacterMontage, InPlayRate);
}

void UTP_WeaponComponent::TryPlayAnimMontage(UAnimMontage* AnimMontage, UAnimMontage* CharacterMontage, float InPlayRate)
{
	if (!GetOwner()->HasAuthority())
		ServerRPCPlayAnimMontage(AnimMontage, CharacterMontage, InPlayRate);
	else
		PlayAnimMontage(AnimMontage, CharacterMontage, InPlayRate);
}

void UTP_WeaponComponent::SpawnFireEffect_Implementation()
{
	//Spawn Emitter & Sound
	UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), FireFlashEmitter, GetSocketLocation("SOCKET_Muzzle")
										, GetSocketRotation("SOCKET_Muzzle") + FRotator(0.0f, 90.0f, 0.0f));
	UGameplayStatics::PlaySoundAtLocation(GetWorld(), FireSound, GetComponentLocation());
}

void UTP_WeaponComponent::ServerRPCSpawnFireEffect_Implementation()
{
	SpawnFireEffect();
}

void UTP_WeaponComponent::TrySpawnFireEffect()
{
	if (!GetOwner()->HasAuthority())
		ServerRPCSpawnFireEffect();
	else
		SpawnFireEffect();
}

void UTP_WeaponComponent::AttachWeapon(AMainCharacterBase* TargetCharacter)
{
	CharacterRef = TargetCharacter;
	if (!CharacterRef.IsValid()) return;
	if (IsValid(CharacterRef.Get()->WeaponRef)) return; 

	// Attach the weapon to the First Person Character
	FAttachmentTransformRules AttachmentRules(EAttachmentRule::SnapToTarget, true);
	AttachToComponent(CharacterRef.Get()->FirstPersonMesh, AttachmentRules, FName(TEXT("SOCKET_Weapon")));

	// switch bHasRifle so the animation blueprint can switch to another animation set
	CharacterRef.Get()->SetHasRifle(true);
	CharacterRef.Get()->WeaponRef = this;
	CharacterRef.Get()->FirstPersonMesh->SetVisibility(true);

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
			EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::TryFire);

			// Reload
			EnhancedInputComponent->BindAction(ReloadAction, ETriggerEvent::Triggered, this, &UTP_WeaponComponent::Reload);

			// Start Aiming
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Started, this, &UTP_WeaponComponent::StartAiming);

			// Stop Aiming
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Canceled, this, &UTP_WeaponComponent::StopAiming);
			EnhancedInputComponent->BindAction(AimAction, ETriggerEvent::Completed, this, &UTP_WeaponComponent::StopAiming);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &UTP_WeaponComponent::StopAiming);
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