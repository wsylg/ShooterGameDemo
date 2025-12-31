// Fill out your copyright notice in the Description page of Project Settings.


#include "ShooterGameMode.h"
#include "Variant_Shooter/ShooterCharacter.h"
#include "Variant_Shooter/ShooterPlayerController.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "PlayerState/ShootPlayerState.h"

AShooterGameMode::AShooterGameMode()
{

}

void AShooterGameMode::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	if(MatchState == MatchState::WaitingPostMatch)
	{
		
	}
}

void AShooterGameMode::PlayerEliminated(AShooterCharacter* ElimmedCharacter, AShooterPlayerController* VictimController, AShooterPlayerController* AttackerController)
{
	AShootPlayerState* AttackerPlayerState = AttackerController ? Cast<AShootPlayerState>(AttackerController->PlayerState) : nullptr;
	AShootPlayerState* VictimPlayerState = VictimController ? Cast<AShootPlayerState>(VictimController->PlayerState) : nullptr;

	if(AttackerPlayerState && AttackerPlayerState != VictimPlayerState)
	{
		AttackerPlayerState->AddToScore(1.f);
	}

	if(ElimmedCharacter)
	{
		ElimmedCharacter->Elim();
		//EndMatch();
	}
}
void AShooterGameMode::RequestRespawn(ACharacter* ElimmedCharacter, AController* ElimmedController)
{
	if(ElimmedCharacter)
	{
		ElimmedCharacter->Reset();
		ElimmedCharacter->Destroy();
	}
	if(ElimmedController)
	{
		TArray<AActor*> PlayerStarts;
		UGameplayStatics::GetAllActorsOfClass(this,APlayerStart::StaticClass(), PlayerStarts);

		int32 Selection = FMath::RandRange(0, PlayerStarts.Num()-1);
		RestartPlayerAtPlayerStart(ElimmedController, PlayerStarts[Selection]);
	}
}

bool AShooterGameMode::ReadyToEndMatch()
{
	return false;
}
