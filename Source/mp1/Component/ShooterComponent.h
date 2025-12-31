

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ShooterComponent.generated.h"
#define TRACE_LENGTH 800000
 
UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MP1_API UShooterComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UShooterComponent();
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	friend class AShooterCharacter;
	friend class AShooterNPC;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	void EquipWeapon(class AShootWeapon* WeaponToEquip);
	void NPCEquipWeapon(class AShootWeapon* WeaponToEquip);
protected:
	// Called when the game starts
	virtual void BeginPlay() override;

	void FireButtonPressed(bool bPressed);

	void Fire();

	UFUNCTION(Server,Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	void TraceUnderCrosshairs(FHitResult& TraceHitResult);

	void SetHUDCrosshairs(float DeltaTime);


private:
	class AShooterCharacter *Character;
	class AShooterNPC* NPC = nullptr;
	//class AShootPlayerController* Controller;
	class AShooterPlayerController* Controller;
	class AShooterHUD* HUD;

	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AShootWeapon* EquippedWeapon;

	bool bFireButtonPressed;

	FVector HitTarget;

	//Automatic fire

	FTimerHandle FireTimer;
	bool bCanFire = true;

	void StartFireTimer();
	void FireTimerFinished();
public:	


		
	
};
