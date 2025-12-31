


#include "ShooterComponent.h"
#include "Weapon/ShootWeapon.h"
#include "ShooterCharacter.h"
#include "Net/UnrealNetwork.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "DrawDebugHelpers.h"
#include "Variant_Shooter/ShooterPlayerController.h"
#include "HUD/ShooterHUD.h"
#include "TimerManager.h"
#include "Variant_Shooter/AI/ShooterNPC.h"

// Sets default values for this component's properties
UShooterComponent::UShooterComponent()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
	
}

void UShooterComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(ThisClass, EquippedWeapon);
}

// Called when the game starts
void UShooterComponent::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}

// Called every frame
void UShooterComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	//FHitResult HitResult;
	//TraceUnderCrosshairs(HitResult);
	// ...
	SetHUDCrosshairs(DeltaTime);
	if(Character && Character->IsLocallyControlled())
	{
		FHitResult HitResult;
		TraceUnderCrosshairs(HitResult);
		HitTarget = HitResult.ImpactPoint;
	}

}

void UShooterComponent::SetHUDCrosshairs(float DeltaTime)
{
	if(Character == nullptr|| Character->Controller == nullptr)return;

	Controller = Controller == nullptr ? Cast<AShooterPlayerController>(Character->Controller): Controller;
	if(Controller)
	{
		HUD = HUD == nullptr? Cast<AShooterHUD>(Controller->GetHUD()):HUD;
		if(HUD)
		{
			FHUDPAckage HUDPackage;
			if(EquippedWeapon)
			{	
				HUDPackage.CrosshairCenter = EquippedWeapon->CrosshairsCenter;
				HUDPackage.CrosshairLeft = EquippedWeapon->CrosshairsLeft;
				HUDPackage.CrosshairRight = EquippedWeapon->CrosshairsRight;
				HUDPackage.CrosshairTop = EquippedWeapon->CrosshairsTop;
				HUDPackage.CrosshairBottom = EquippedWeapon->CrosshairsBottom;

			}
			else
			{
				HUDPackage.CrosshairCenter = nullptr;
				HUDPackage.CrosshairLeft = nullptr;
				HUDPackage.CrosshairRight = nullptr;
				HUDPackage.CrosshairTop = nullptr;
				HUDPackage.CrosshairBottom = nullptr;
			}
			HUD->SetHUDPackage(HUDPackage);

		}
	}
}

void UShooterComponent::StartFireTimer()
{
	if(EquippedWeapon== nullptr||Character == nullptr)return;
	Character->GetWorldTimerManager().SetTimer(
		FireTimer,
		this,
		&UShooterComponent::FireTimerFinished,
		EquippedWeapon->FireDelay
	);

}

void UShooterComponent::FireTimerFinished()
{
	if (EquippedWeapon == nullptr || Character == nullptr)return;
	bCanFire = true;
	if(bFireButtonPressed && EquippedWeapon->bAutomatic)
	{
		Fire();
	}
}

void UShooterComponent::FireButtonPressed(bool bPressed)
{
	bFireButtonPressed = bPressed;
	if(bFireButtonPressed && EquippedWeapon)
	{
		//FHitResult HitResult;
		//TraceUnderCrosshairs(HitResult);
		Fire();
	}
	
}

void UShooterComponent::Fire()
{
	if (bCanFire)
	{
		bCanFire = false;
		ServerFire(HitTarget);
		StartFireTimer();
	}

}

void UShooterComponent::ServerFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	MulticastFire(TraceHitTarget);
}

void UShooterComponent::MulticastFire_Implementation(const FVector_NetQuantize& TraceHitTarget)
{
	if (EquippedWeapon == nullptr) return;
	if (Character)
	{
		EquippedWeapon->Fire(TraceHitTarget);
	}
}

void UShooterComponent::EquipWeapon(AShootWeapon* WeaponToEquip)
{
	if(Character == nullptr || WeaponToEquip == nullptr) return;

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	Character->AttachShootWeaponMeshes(EquippedWeapon);
	EquippedWeapon->SetOwner(Character);

}

void UShooterComponent::NPCEquipWeapon(AShootWeapon* WeaponToEquip)
{
	if (NPC == nullptr || WeaponToEquip == nullptr) return;

	EquippedWeapon = WeaponToEquip;
	EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
	NPC->AttachShootWeaponMeshes(EquippedWeapon);
	EquippedWeapon->SetOwner(NPC);

}

void UShooterComponent::OnRep_EquippedWeapon()
{
	if(EquippedWeapon && Character)
	{
		EquippedWeapon->SetWeaponState(EWeaponState::EWS_Equipped);
		Character->AttachShootWeaponMeshes(EquippedWeapon);
	}
}

void UShooterComponent::TraceUnderCrosshairs(FHitResult& TraceHitResult)
{
	FVector2D ViewportSize;
	if (GEngine && GEngine->GameViewport)
	{
		GEngine->GameViewport->GetViewportSize(ViewportSize);
	}
	FVector2D CrosshairLocation(ViewportSize.X / 2.f, ViewportSize.Y / 2.f);
	FVector  CrosshairWorldPosition;
	FVector CrosshairWorldDirection;
	bool bScreenToWorld = UGameplayStatics::DeprojectScreenToWorld(
		UGameplayStatics::GetPlayerController(this, 0),
		CrosshairLocation,
		CrosshairWorldPosition,
		CrosshairWorldDirection
	);
	if (bScreenToWorld)
	{
		FVector Start = CrosshairWorldPosition;

		FVector End = Start + CrosshairWorldDirection * TRACE_LENGTH;

		GetWorld()->LineTraceSingleByChannel(
			TraceHitResult,
			Start,
			End,
			ECollisionChannel::ECC_Visibility
		);
		//if(!TraceHitResult.bBlockingHit)
		//{
		//	TraceHitResult.ImpactPoint = End;
		//	HitTarget = End;
		//}
		//else
		//{	HitTarget = TraceHitResult.ImpactPoint;
		//	//DrawDebugSphere(
		//	//	GetWorld(),
		//	//	TraceHitResult.ImpactPoint,
		//	//	12.f,
		//	//	12,
		//	//	FColor::Red
		//	//);
		//}

	}
}