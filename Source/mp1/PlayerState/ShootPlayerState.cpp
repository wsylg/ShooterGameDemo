// Fill out your copyright notice in the Description page of Project Settings.


#include "ShootPlayerState.h"
#include "Variant_Shooter/ShooterCharacter.h"
#include "Variant_Shooter/ShooterPlayerController.h"


void AShootPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(GetScore()+ScoreAmount);
	//Score += ScoreAmount;s
	Character = Character == nullptr ? Cast<AShooterCharacter>(GetPawn()) : Character;
	if (Character)
	{
		Controller = Controller == nullptr ? Cast<AShooterPlayerController>(Character->Controller) : Controller;
		if (Controller)
		{
			Controller->SetHUDScore(GetScore());
			if(GetScore()>=3)
			{
				for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
				{
					AShooterPlayerController* PC = Cast<AShooterPlayerController>(*It);
					if (PC)
					{
						// 逐个调用组播函数 → 每个客户端执行
						PC->SetEndHUD();
					}
				}
			}
		}

	}
}


void AShootPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	//GetPawn();

	Character = Character == nullptr ? Cast<AShooterCharacter>(GetPawn()) : Character;
	if(Character)
	{
		Controller = Controller == nullptr ? Cast<AShooterPlayerController>(Character->Controller) : Controller;
		if(Controller)
		{
			Controller->SetHUDScore(GetScore());
		}

	}
}
