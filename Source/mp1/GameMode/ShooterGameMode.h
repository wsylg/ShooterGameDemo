// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameMode.h"
#include "ShooterGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MP1_API AShooterGameMode : public AGameMode
{
	GENERATED_BODY()

public:
	AShooterGameMode();
	virtual void Tick(float DeltaTime) override;
	virtual void  PlayerEliminated(class AShooterCharacter* ElimmedCharacter, class AShooterPlayerController* VictimController, class AShooterPlayerController* AttackerController);
	virtual void RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController);
	virtual bool ReadyToEndMatch();
};
