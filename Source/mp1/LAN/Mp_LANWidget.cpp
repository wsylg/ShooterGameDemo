// Fill out your copyright notice in the Description page of Project Settings.


#include "LAN/Mp_LANWidget.h"
#include "Components/Button.h"
#include "Components/EditableTextBox.h"
#include <Kismet/GameplayStatics.h>

void UMp_LANWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	FInputModeUIOnly InputMode;
	GetOwningPlayer()->SetInputMode(InputMode);
	GetOwningPlayer()->SetShowMouseCursor(true);



	Button_Host->OnClicked.AddDynamic(this,&ThisClass::HostButtonClicked);

	Button_Join->OnClicked.AddDynamic(this, &ThisClass::JoinButtonClicked);
}

void UMp_LANWidget::HostButtonClicked()
{
	FInputModeGameOnly InputMode;
	GetOwningPlayer()->SetInputMode(InputMode);
	GetOwningPlayer()->SetShowMouseCursor(false);

	UGameplayStatics::OpenLevelBySoftObjectPtr(this,HostingLevel,true,TEXT("listen"));
}

void UMp_LANWidget::JoinButtonClicked()
{
	FInputModeGameOnly InputMode;
	GetOwningPlayer()->SetInputMode(InputMode);
	GetOwningPlayer()->SetShowMouseCursor(false);


	const FString Address = TextBox_IpAddress->GetText().ToString();
	UGameplayStatics::OpenLevel(this,*Address);

}
