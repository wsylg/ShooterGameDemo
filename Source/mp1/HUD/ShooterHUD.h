// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "ShooterHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPAckage
{
	GENERATED_BODY()
public:
	class UTexture2D* CrosshairCenter;
	UTexture2D* CrosshairLeft;
	UTexture2D* CrosshairRight;
	UTexture2D* CrosshairTop;
	UTexture2D* CrosshairBottom;
};
/**
 * 
 */
UCLASS()
class MP1_API AShooterHUD : public AHUD
{
	GENERATED_BODY()

public:
	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category = "Play Stats")
	TSubclassOf<class UUserWidget>CharacterOverlayClass;

	class UCharacterOverlay* CharacterOverlay;

	UPROPERTY(EditAnywhere, Category = "Play Stats")
	TSubclassOf<class UUserWidget> EndWidgetClass;

	class UEndWidget* EndWidget;

	void AddEndGame();

protected:

	virtual void BeginPlay() override;

	void AddCharacterOverlay();

private:
	FHUDPAckage HUDPackage;
	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter);
public:
	FORCEINLINE void SetHUDPackage(const FHUDPAckage& Package){ HUDPackage = Package;}
	
	
};
