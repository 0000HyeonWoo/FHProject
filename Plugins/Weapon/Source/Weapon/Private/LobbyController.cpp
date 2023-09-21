// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyController.h"
#include "Blueprint/UserWidget.h"
#include "MyGameInstance.h"
#include "Kismet/GamePlaystatics.h"

void ALobbyController::BeginPlay()
{
	Super::BeginPlay();

	if (IsLocalPlayerController() == false)
	{
		return;
	}

	LobbyWidget = CreateWidget<UUserWidget>(GetWorld(), LobbyWidgetClass);
	LobbyWidget->AddToViewport();

	APlayerController* player0 = Cast<APlayerController>(GetWorld()->GetFirstPlayerController());

	if (player0)
	{
		player0->SetInputMode(FInputModeGameAndUI());
		player0->bShowMouseCursor = true;
	}


}