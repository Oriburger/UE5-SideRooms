// Fill out your copyright notice in the Description page of Project Settings.


#include "../public/SideRoomPlayerController.h"
#include "../public/MainCharacterBase.h"
#include "Kismet/GameplayStatics.h"
#include "Blueprint/WidgetBlueprintLibrary.h"
#include "Blueprint/UserWidget.h"

void ASideRoomPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	InitWidget();
}

void ASideRoomPlayerController::DisconnectGame_Implementation()
{
	UGameplayStatics::OpenLevel(GetWorld(), "MainMenu", true, "Failed");
}

void ASideRoomPlayerController::InitWidget_Implementation()
{
	if (DefaultWidgetClass)
	{
		UUserWidget * widgetInstance = CreateWidget<UUserWidget>(this, DefaultWidgetClass);
		if (widgetInstance)
		{
			widgetInstance->AddToViewport();
		}
	}
	UWidgetBlueprintLibrary::SetInputMode_GameOnly(this);
}
