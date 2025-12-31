


#include "ShootWeapon.h"
#include "Components/SphereComponent.h"
#include "Components/WidgetComponent.h"
#include "Variant_Shooter/ShooterCharacter.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AShootWeapon::AShootWeapon()
{

	PrimaryActorTick.bCanEverTick = true;

	bNetLoadOnClient = true;
	bReplicates = true;
	SetReplicateMovement(true);


	ThirdPersonMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("ThirdMesh"));

	SetRootComponent(ThirdPersonMesh);


	ThirdPersonMesh->SetupAttachment(WeaponMesh);
	ThirdPersonMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	ThirdPersonMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	ThirdPersonMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//ThirdPersonMesh->bOwnerNoSee = true;
	//RootComponent = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
	WeaponMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("WeaponMesh"));

	WeaponMesh->SetupAttachment(ThirdPersonMesh);

	WeaponMesh->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Block);
	WeaponMesh->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Ignore);
	WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	//WeaponMesh->SetFirstPersonPrimitiveType(EFirstPersonPrimitiveType::FirstPerson);
	//WeaponMesh->bOnlyOwnerSee = true;

	AreaSphere = CreateDefaultSubobject<USphereComponent>(TEXT("AreaSphere"));
	AreaSphere->SetupAttachment(WeaponMesh);
	AreaSphere->SetCollisionResponseToAllChannels(ECollisionResponse::ECR_Ignore);
	AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);

	PickupWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("PickupWidget"));
	PickupWidget->SetupAttachment(ThirdPersonMesh);


	//ThirdPersonMesh->SetFirstPersonPrimitiveType(EFirstPersonPrimitiveType::WorldSpaceRepresentation);



}

// Called when the game starts or when spawned
void AShootWeapon::BeginPlay()
{
	Super::BeginPlay();



	if (HasAuthority())
	{
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		AreaSphere->SetCollisionResponseToChannel(ECollisionChannel::ECC_Pawn, ECollisionResponse::ECR_Overlap);
		AreaSphere->OnComponentBeginOverlap.AddDynamic(this, &AShootWeapon::OnSphereOverlap);
		AreaSphere->OnComponentEndOverlap.AddDynamic(this,&AShootWeapon::OnSphereEndOverlap);
	}
	if (PickupWidget)
	{
		PickupWidget->SetVisibility(false);
	}
}
// Called every frame
void AShootWeapon::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AShootWeapon::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherAcotr, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherAcotr);
	if(ShooterCharacter)
	{	
		//PickupWidget->SetVisibility(true);
		ShooterCharacter->SetOverlappingWeapon(this);
	}
}

void AShootWeapon::OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherAcotr, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(OtherAcotr);
	if (ShooterCharacter)
	{
		//PickupWidget->SetVisibility(true);
		ShooterCharacter->SetOverlappingWeapon(nullptr);
	}
}

void AShootWeapon::OnRep_WeaponState()
{
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ThirdPersonMesh->SetSimulatePhysics(false);
		ThirdPersonMesh->SetEnableGravity(false);
		ThirdPersonMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;

	case EWeaponState::EWS_Dropped:
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ThirdPersonMesh->SetSimulatePhysics(true);
		ThirdPersonMesh->SetEnableGravity(true);
		ThirdPersonMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}
}

void AShootWeapon::SetWeaponState(EWeaponState state)
{
	WeaponState = state;
	switch (WeaponState)
	{
	case EWeaponState::EWS_Equipped:
		ShowPickupWidget(false);
		AreaSphere->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		WeaponMesh->SetSimulatePhysics(false);
		WeaponMesh->SetEnableGravity(false);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		ThirdPersonMesh->SetSimulatePhysics(false);
		ThirdPersonMesh->SetEnableGravity(false);
		ThirdPersonMesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
		break;
	case EWeaponState::EWS_Dropped:
		if(HasAuthority())
		{
			AreaSphere->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
		}
		WeaponMesh->SetSimulatePhysics(true);
		WeaponMesh->SetEnableGravity(true);
		WeaponMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		ThirdPersonMesh->SetSimulatePhysics(true);
		ThirdPersonMesh->SetEnableGravity(true);
		ThirdPersonMesh->SetCollisionEnabled(ECollisionEnabled::QueryAndPhysics);
		break;
	}

}



void AShootWeapon::ShowPickupWidget(bool bShowWidget)
{
	if(PickupWidget)
	{
		PickupWidget->SetVisibility(bShowWidget);
	}
}

void AShootWeapon::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ThisClass ,WeaponState);
}
void AShootWeapon::Fire(const FVector& HitTarget)
{

}

void AShootWeapon::Dropped()
{
	SetWeaponState(EWeaponState::EWS_Dropped);
	FDetachmentTransformRules DetachRules(EDetachmentRule::KeepWorld,true);
	this->DetachFromActor(DetachRules);
	WeaponMesh->DetachFromComponent(DetachRules);
	//ThirdPersonMesh->DetachFromComponent(DetachRules);

	SetOwner(nullptr);
}
