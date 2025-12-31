// Copyright Epic Games, Inc. All Rights Reserved.


#include "Variant_Shooter/ShooterPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/PlayerStart.h"
#include "ShooterCharacter.h"
#include "ShooterBulletCounterUI.h"
#include "mp1.h"
#include "Widgets/Input/SVirtualJoystick.h"
#include "HUD/ShooterHUD.h"
#include "HUD/CharacterOverlay.h"
#include "HUD/EndWidget.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "ShooterCharacter.h"

void AShooterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (IsLocalPlayerController())
	{
		if (SVirtualJoystick::ShouldDisplayTouchInterface())
		{
			// spawn the mobile controls widget
			MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

			if (MobileControlsWidget)
			{
				// add the controls to the player screen
				MobileControlsWidget->AddToPlayerScreen(0);

			} else {

				UE_LOG(Logmp1, Error, TEXT("Could not spawn mobile controls widget."));

			}
		}
		ShooterHUD = Cast<AShooterHUD>(GetHUD());
		//// create the bullet counter widget and add it to the screen
		//BulletCounterUI = CreateWidget<UShooterBulletCounterUI>(this, BulletCounterUIClass);

		//if (BulletCounterUI)
		//{
		//	BulletCounterUI->AddToPlayerScreen(0);

		//} else {

		//	UE_LOG(Logmp1, Error, TEXT("Could not spawn bullet counter widget."));

		//}
		
	}
}

void AShooterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(InPawn);
	if(ShooterCharacter)
	{
		SetHUDHealth(ShooterCharacter->GetCurrentHP(), ShooterCharacter->GetMAXHP());
	}
}

void AShooterPlayerController::SetupInputComponent()
{
	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// add the input mapping contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!SVirtualJoystick::ShouldDisplayTouchInterface())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}
	}
}

//void AShooterPlayerController::OnPossess(APawn* InPawn)
//{
//	Super::OnPossess(InPawn);
//
//	// subscribe to the pawn's OnDestroyed delegate
//	InPawn->OnDestroyed.AddDynamic(this, &AShooterPlayerController::OnPawnDestroyed);
//
//	// is this a shooter character?
//	if (AShooterCharacter* ShooterCharacter = Cast<AShooterCharacter>(InPawn))
//	{
//		// add the player tag
//		ShooterCharacter->Tags.Add(PlayerPawnTag);
//
//		// subscribe to the pawn's delegates
//		ShooterCharacter->OnBulletCountUpdated.AddDynamic(this, &AShooterPlayerController::OnBulletCountUpdated);
//		ShooterCharacter->OnDamaged.AddDynamic(this, &AShooterPlayerController::OnPawnDamaged);
//
//		// force update the life bar
//		ShooterCharacter->OnDamaged.Broadcast(1.0f);
//	}
//}

//void AShooterPlayerController::OnPawnDestroyed(AActor* DestroyedActor)
//{
//	// reset the bullet counter HUD
//	BulletCounterUI->BP_UpdateBulletCounter(0, 0);
//
//	// find the player start
//	TArray<AActor*> ActorList;
//	UGameplayStatics::GetAllActorsOfClass(GetWorld(), APlayerStart::StaticClass(), ActorList);
//
//	if (ActorList.Num() > 0)
//	{
//		// select a random player start
//		AActor* RandomPlayerStart = ActorList[FMath::RandRange(0, ActorList.Num() - 1)];
//
//		// spawn a character at the player start
//		const FTransform SpawnTransform = RandomPlayerStart->GetActorTransform();
//
//		if (AShooterCharacter* RespawnedCharacter = GetWorld()->SpawnActor<AShooterCharacter>(CharacterClass, SpawnTransform))
//		{
//			// possess the character
//			Possess(RespawnedCharacter);
//		}
//	}
//}

//void AShooterPlayerController::OnBulletCountUpdated(int32 MagazineSize, int32 Bullets)
//{
//	// update the UI
//	if (BulletCounterUI)
//	{
//		BulletCounterUI->BP_UpdateBulletCounter(MagazineSize, Bullets);
//	}
//}
//
//void AShooterPlayerController::OnPawnDamaged(float LifePercent)
//{
//	if (IsValid(BulletCounterUI))
//	{
//		BulletCounterUI->BP_Damaged(LifePercent);
//	}
//}

void AShooterPlayerController::SetHUDHealth(float CurrentHP, float MaxHP)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;
	bool bHUDValid = ShooterHUD && 
		ShooterHUD->CharacterOverlay && 
		ShooterHUD->CharacterOverlay->HealthBar && 
		ShooterHUD->CharacterOverlay->HealthText;
	if(bHUDValid)
	{
		const float  HealthPercent = CurrentHP / MaxHP;
		ShooterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
		FString HealthText = FString::Printf(TEXT("%d/%d"),FMath::CeilToInt(CurrentHP), FMath::CeilToInt(MaxHP));
		ShooterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
	}
}

void AShooterPlayerController::SetHUDScore(float Score)
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;
	bool bHUDValid = ShooterHUD && 
		ShooterHUD->CharacterOverlay &&
		ShooterHUD->CharacterOverlay->ScoreAmount;
	if(bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"),FMath::FloorToInt(Score));
		ShooterHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
	}
}

void AShooterPlayerController::SetEndHUD_Implementation()
{
	ShooterHUD = ShooterHUD == nullptr ? Cast<AShooterHUD>(GetHUD()) : ShooterHUD;
	bool bHUDValid = ShooterHUD &&
		ShooterHUD->EndWidget &&
		ShooterHUD->EndWidget->EndText;
	if (bHUDValid)
	{
		ShooterHUD->AddEndGame();
	}
}