// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ShootWeapon.h"
#include "ShootProjectile.generated.h"

/**
 * 
 */
UCLASS()
class MP1_API AShootProjectile : public AShootWeapon
{
	GENERATED_BODY()
	
public:
	
	virtual void Fire(const FVector& HitTarget) override;

private:

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> ProjectileClass;
	
	
};
