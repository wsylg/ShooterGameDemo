// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootProjectile.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Projectile.h"

void AShootProjectile::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	if(!HasAuthority())return;
	APawn * InstigatorPawn  = Cast<APawn>(GetOwner());
	const USkeletalMeshSocket* FirstPersonMuzzleSocket  = GetFirstPersonMesh()->GetSocketByName(FName("Muzzle"));
	const USkeletalMeshSocket* ThirdPersonMuzzleSocket = GetFirstPersonMesh()->GetSocketByName(FName("Muzzle"));
	if(FirstPersonMuzzleSocket&&ThirdPersonMuzzleSocket)
	{
		FTransform SocketTransform = FirstPersonMuzzleSocket->GetSocketTransform(GetFirstPersonMesh());

		FVector ToTarget  = HitTarget - SocketTransform.GetLocation();
		FRotator TargetRotation = ToTarget.Rotation();
		if(ProjectileClass && InstigatorPawn)
		{	
			FActorSpawnParameters SpawnParams;
			SpawnParams.Owner = GetOwner();
			SpawnParams.Instigator = InstigatorPawn;
			UWorld* World = GetWorld();
			if(World)
			{	
				World->SpawnActor<AProjectile>(
					ProjectileClass,
					SocketTransform.GetLocation(),
					TargetRotation,
					SpawnParams
				);
			}
		}
		
	}

}
