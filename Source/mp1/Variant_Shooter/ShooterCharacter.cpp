// Copyright Epic Games, Inc. All Rights Reserved.


#include "ShooterCharacter.h"
#include "ShooterWeapon.h"
#include "EnhancedInputComponent.h"
#include "Components/InputComponent.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Engine/World.h"
#include "Camera/CameraComponent.h"
#include "TimerManager.h"
#include "GameMode/ShooterGameMode.h"
#include "Net/UnrealNetwork.h"
#include "Engine/Engine.h"
#include "Component/ShooterComponent.h"
#include "Weapon/ShootWeapon.h"
#include "Variant_Shooter/ShooterPlayerController.h"
#include "Components/CapsuleComponent.h"
#include "TimerManager.h"
#include "PlayerState/ShootPlayerState.h"

AShooterCharacter::AShooterCharacter()
{
	// create the noise emitter component
	PawnNoiseEmitter = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("Pawn Noise Emitter"));
	SpawnCollisionHandlingMethod = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;
	// configure movement
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 600.0f, 0.0f);
	Shoot = CreateDefaultSubobject<UShooterComponent>(TEXT("ShooterComponent"));
	Shoot->SetIsReplicated(true);
}

void AShooterCharacter::BeginPlay()
{
	Super::BeginPlay();

	// update the HUD
	UpdateHUDHealth();
	//OnDamaged.Broadcast(1.0f);

	if(HasAuthority())
	{
		OnTakeAnyDamage.AddDynamic(this, &AShooterCharacter::ReceiveDamage);
	}
}



void AShooterCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	//UpdateHUDHealth();
	PollInit();
}

void AShooterCharacter::EndPlay(EEndPlayReason::Type EndPlayReason)
{
	Super::EndPlay(EndPlayReason);

	// clear the respawn timer
	GetWorld()->GetTimerManager().ClearTimer(RespawnTimer);
}

void AShooterCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// base class handles move, aim and jump inputs
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent))
	{
		// Firing
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Started, this, &AShooterCharacter::DoFireAction);
		EnhancedInputComponent->BindAction(FireAction, ETriggerEvent::Completed, this, &AShooterCharacter::DoStopFireAction);

		// Switch weapon
		EnhancedInputComponent->BindAction(SwitchWeaponAction, ETriggerEvent::Triggered, this, &AShooterCharacter::DoSwitchWeapon);
	
		//test currentHP
		EnhancedInputComponent->BindAction(GeneralInput, ETriggerEvent::Started, this, &AShooterCharacter::OnGeneralInput);
		EnhancedInputComponent->BindAction(EquipInput, ETriggerEvent::Started, this, &AShooterCharacter::EquipWeaponAction);
	}

}

//float AShooterCharacter::TakeDamage(float Damage, struct FDamageEvent const& DamageEvent, AController* EventInstigator, AActor* DamageCauser)
//{
//	// ignore if already dead
//
//	if (CurrentHP <= 0.0f)
//	{
//		return 0.0f;
//	}
//	//float damageApplied = CurrentHealth - Damage;
//	// Reduce HP
//	CurrentHP -= Damage;
//
//	// Have we depleted HP?
//	if (CurrentHP <= 0.0f)
//	{
//		//Die();
//		AShooterGameMode* ShooterGameMode = GetWorld()->GetAuthGameMode<AShooterGameMode>();
//		if(ShooterGameMode)
//		{
//			ShooterPlayerController = ShooterPlayerController ==nullptr ? Cast<AShooterPlayerController>(Controller):ShooterPlayerController;
//			AShooterPlayerController* AttackerController = Cast<AShooterPlayerController>(EventInstigator);
//			ShooterGameMode->PlayerEliminated(this, ShooterPlayerController, AttackerController);
//		}
//	}
//
//	// update the HUD
//	OnDamaged.Broadcast(FMath::Max(0.0f, CurrentHP / MaxHP));
//	//SetCurrentHealth(damageApplied);
//
//	return Damage;
//	//return damageApplied;
//}

void AShooterCharacter::DoStartFiring()
{
	// fire the current weapon
	if (CurrentWeapon)
	{
		CurrentWeapon->StartFiring();
	}
}

void AShooterCharacter::DoStopFiring()
{
	// stop firing the current weapon
	if (CurrentWeapon)
	{
		CurrentWeapon->StopFiring();
	}
}

void AShooterCharacter::DoSwitchWeapon()
{
	// ensure we have at least two weapons two switch between
	if (OwnedWeapons.Num() > 1)
	{
		// deactivate the old weapon
		CurrentWeapon->DeactivateWeapon();

		// find the index of the current weapon in the owned list
		int32 WeaponIndex = OwnedWeapons.Find(CurrentWeapon);

		// is this the last weapon?
		if (WeaponIndex == OwnedWeapons.Num() - 1)
		{
			// loop back to the beginning of the array
			WeaponIndex = 0;
		}
		else {
			// select the next weapon index
			++WeaponIndex;
		}

		// set the new weapon as current
		CurrentWeapon = OwnedWeapons[WeaponIndex];

		// activate the new weapon
		CurrentWeapon->ActivateWeapon();
	}
}

void AShooterCharacter::AttachWeaponMeshes(AShooterWeapon* Weapon)
{
	if(HasAuthority())
	{
		const FAttachmentTransformRules AttachmentRule(EAttachmentRule::SnapToTarget, false);

		// attach the weapon actor
		Weapon->AttachToActor(this, AttachmentRule);

		// attach the weapon meshes
		Weapon->GetFirstPersonMesh()->AttachToComponent(GetFirstPersonMesh(), AttachmentRule, FirstPersonWeaponSocket);
		Weapon->GetThirdPersonMesh()->AttachToComponent(GetMesh(), AttachmentRule, FirstPersonWeaponSocket);
	}
	
}

void AShooterCharacter::AttachShootWeaponMeshes(AShootWeapon* Weapon)
{

		const FAttachmentTransformRules AttachmentRule(EAttachmentRule::SnapToTarget, false);

		// attach the weapon actor
		Weapon->AttachToActor(this, AttachmentRule);

		// attach the weapon meshes
		Weapon->GetFirstPersonMesh()->AttachToComponent(GetFirstPersonMesh(), AttachmentRule, FirstPersonWeaponSocket);
		Weapon->GetThirdPersonMesh()->AttachToComponent(GetMesh(), AttachmentRule, FirstPersonWeaponSocket);


}

bool AShooterCharacter::IsWeaponEquipped()
{
	return (Shoot&&Shoot->EquippedWeapon);
}

void AShooterCharacter::Elim()
{
	if(Shoot && Shoot->EquippedWeapon)
	{
		Shoot->EquippedWeapon->Dropped();
	}
	MulticastElim();
	GetWorldTimerManager().SetTimer(
		ElimTimer,
		this,
		&AShooterCharacter::ElimTimerFinished,
		ElimDealy
	);
	
}

void AShooterCharacter::MulticastElim_Implementation()
{
	bElimmed = true;
	PlayElimMontage();

	//禁用角色移动
	GetCharacterMovement()->DisableMovement();
	GetCharacterMovement()->StopMovementImmediately();
	if(ShooterPlayerController)
	{
		DisableInput(ShooterPlayerController);
	}
	//禁用碰撞
	GetCapsuleComponent()->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	GetMesh()->SetCollisionEnabled(ECollisionEnabled::NoCollision);

}

void AShooterCharacter::ElimTimerFinished()
{
	AShooterGameMode* ShooterGameMode = GetWorld()->GetAuthGameMode<AShooterGameMode>();
	if(ShooterGameMode)
	{
		ShooterGameMode->RequestRespawn(this, Controller);
	}
}

FVector AShooterCharacter::GetHitTarget() const
{
	if (Shoot == nullptr )return FVector();
	return Shoot->HitTarget;
}

void AShooterCharacter::PlayFiringMontage(UAnimMontage* Montage)
{
	
}

void AShooterCharacter::AddWeaponRecoil(float Recoil)
{
	// apply the recoil as pitch input
	AddControllerPitchInput(Recoil);
}

void AShooterCharacter::UpdateWeaponHUD(int32 CurrentAmmo, int32 MagazineSize)
{
	OnBulletCountUpdated.Broadcast(MagazineSize, CurrentAmmo);
}

FVector AShooterCharacter::GetWeaponTargetLocation()
{
	// trace ahead from the camera viewpoint
	FHitResult OutHit;

	const FVector Start = GetFirstPersonCameraComponent()->GetComponentLocation();
	const FVector End = Start + (GetFirstPersonCameraComponent()->GetForwardVector() * MaxAimDistance);

	FCollisionQueryParams QueryParams;
	QueryParams.AddIgnoredActor(this);

	GetWorld()->LineTraceSingleByChannel(OutHit, Start, End, ECC_Visibility, QueryParams);

	// return either the impact point or the trace end
	return OutHit.bBlockingHit ? OutHit.ImpactPoint : OutHit.TraceEnd;
}

void AShooterCharacter::AddWeaponClass(const TSubclassOf<AShooterWeapon>& WeaponClass)
{
	if(!HasAuthority())
	{
		//Server_AddWeaponClass(WeaponClass);
	}
	// do we already own this weapon?
	AShooterWeapon* OwnedWeapon = FindWeaponOfType(WeaponClass);

	if (!OwnedWeapon)
	{
		// spawn the new weapon
		FActorSpawnParameters SpawnParams;
		SpawnParams.Owner = this;
		SpawnParams.Instigator = this;
		SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
		SpawnParams.TransformScaleMethod = ESpawnActorScaleMethod::MultiplyWithRoot;

		AShooterWeapon* AddedWeapon = GetWorld()->SpawnActor<AShooterWeapon>(WeaponClass, GetActorTransform(), SpawnParams);

		if (AddedWeapon)
		{
			// add the weapon to the owned list
			OwnedWeapons.Add(AddedWeapon);

			// if we have an existing weapon, deactivate it
			if (CurrentWeapon)
			{
				CurrentWeapon->DeactivateWeapon();
			}

			// switch to the new weapon
			CurrentWeapon = AddedWeapon;
			CurrentWeapon->ActivateWeapon();
		}
	}
}

void AShooterCharacter::OnWeaponActivated(AShooterWeapon* Weapon)
{
	// update the bullet counter
	OnBulletCountUpdated.Broadcast(Weapon->GetMagazineSize(), Weapon->GetBulletCount());

	// set the character mesh AnimInstances
	GetFirstPersonMesh()->SetAnimInstanceClass(Weapon->GetFirstPersonAnimInstanceClass());
	GetMesh()->SetAnimInstanceClass(Weapon->GetThirdPersonAnimInstanceClass());
}

void AShooterCharacter::OnWeaponDeactivated(AShooterWeapon* Weapon)
{
	// unused
}

void AShooterCharacter::OnSemiWeaponRefire()
{
	// unused
}

void AShooterCharacter::PlayElimMontage()
{
	UAnimInstance* AnimInstance =GetMesh()->GetAnimInstance();
	if(AnimInstance && ElimMontage)
	{
		AnimInstance->Montage_Play(ElimMontage);

	}
}

AShooterWeapon* AShooterCharacter::FindWeaponOfType(TSubclassOf<AShooterWeapon> WeaponClass) const
{
	// check each owned weapon
	for (AShooterWeapon* Weapon : OwnedWeapons)
	{
		if (Weapon->IsA(WeaponClass))
		{
			return Weapon;
		}
	}

	// weapon not found
	return nullptr;

}

void AShooterCharacter::OnRespawn()
{
	// destroy the character to force the PC to respawn
	Destroy();
}

void AShooterCharacter::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass,CurrentHP);

	DOREPLIFETIME_CONDITION(ThisClass, OverlappingWeapon,COND_OwnerOnly);
}

void AShooterCharacter::PostInitializeComponents()
{
	Super::PostInitializeComponents();
	if(Shoot)
	{
		Shoot->Character = this;
	}
}


void AShooterCharacter::DoFireAction()
{
	if(Shoot)
	{
		Shoot->FireButtonPressed(true);
	}
}

void AShooterCharacter::DoStopFireAction()
{
	if (Shoot)
	{
		Shoot->FireButtonPressed(false);
	}
}

void AShooterCharacter::OnGeneralInput()
{
	GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Green, FString::Printf(TEXT("CurrentHP: %f"), CurrentHP));
}

void AShooterCharacter::EquipWeaponAction()
{
	if(Shoot)
	{	
		if(HasAuthority())
		{
			Shoot->EquipWeapon(OverlappingWeapon);
		}
		else
		{	
			ServerEquipWeaponAction();
		}
		
	}
}

void AShooterCharacter::ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatorController, AActor* DamageCauser)
{
	CurrentHP = FMath::Clamp(CurrentHP - Damage, 0.f , MaxHP);
	UpdateHUDHealth();

	if (CurrentHP == 0.f)
	{
		AShooterGameMode* ShooterGameMode = GetWorld()->GetAuthGameMode<AShooterGameMode>();
		if (ShooterGameMode)
		{
			ShooterPlayerController = ShooterPlayerController == nullptr ? Cast<AShooterPlayerController>(Controller) : ShooterPlayerController;
			AShooterPlayerController* AttackerController = Cast<AShooterPlayerController>(InstigatorController);
			ShooterGameMode->PlayerEliminated(this, ShooterPlayerController, AttackerController);
		}
	}
	
}


void AShooterCharacter::OnRep_CurrentHP()
{
	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Orange, FString::Printf(TEXT("CurrentHP: %f"), CurrentHP));
	UpdateHUDHealth();
}

void AShooterCharacter::UpdateHUDHealth()
{
	ShooterPlayerController = ShooterPlayerController == nullptr ? Cast<AShooterPlayerController>(Controller) : ShooterPlayerController;
	if (ShooterPlayerController)
	{
		ShooterPlayerController->SetHUDHealth(CurrentHP, MaxHP);
	}
}

void AShooterCharacter::PollInit()
{
	if(ShootPlayerState == nullptr)
	{
		ShootPlayerState = GetPlayerState<AShootPlayerState>();
		if(ShootPlayerState)
		{
			ShootPlayerState->AddToScore(0.f);
		}
	}
}

void AShooterCharacter::OnRep_OverlappingWeapon(AShootWeapon* LastWeapon)
{
	if(OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(true);
	}
	if(LastWeapon)
	{
		LastWeapon->ShowPickupWidget(false);
	}
}

void AShooterCharacter::ServerEquipWeaponAction_Implementation()
{
	if (Shoot)
	{
		Shoot->EquipWeapon(OverlappingWeapon);
	}
}


void AShooterCharacter::SetOverlappingWeapon(AShootWeapon* Weapon)
{
	if (OverlappingWeapon)
	{
		OverlappingWeapon->ShowPickupWidget(false);
	}
	OverlappingWeapon  = Weapon;
	if(IsLocallyControlled())
	{
		if (OverlappingWeapon)
		{
			OverlappingWeapon->ShowPickupWidget(true);
		}
	}
}


//void AShooterCharacter::Server_AddWeaponClass_Implementation(const TSubclassOf<AShooterWeapon>& WeaponClass)
//{
//	AddWeaponClass(WeaponClass); // 服务器执行实际添加
//}