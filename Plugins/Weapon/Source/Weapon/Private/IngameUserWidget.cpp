// Fill out your copyright notice in the Description page of Project Settings.


#include "IngameUserWidget.h"
#include "Components/ScrollBox.h"
#include "Components/EditableTextBox.h"
#include "Components/TextBlock.h"
#include "Components/Button.h"
#include "LobbyController.h"
#include "MyGameInstance.h"
#include "Kismet/GamePlaystatics.h"

void UIngameUserWidget::NativeConstruct()
{
	Super::NativeConstruct();

	InputBox = Cast<UEditableTextBox>(GetWidgetFromName(TEXT("Inputbox")));
	if (InputBox)
	{
		InputBox->OnTextChanged.AddDynamic(this, &UIngameUserWidget::OnChangedText);
		InputBox->OnTextCommitted.AddDynamic(this, &UIngameUserWidget::OnCommitedText);
	}

	StartGameButton = Cast<UButton>(GetWidgetFromName(TEXT("StartGame")));
	if (StartGameButton)
	{
		StartGameButton->OnClicked.AddDynamic(this, &UIngameUserWidget::StartGame);
	}

	ScrollBox = Cast<UScrollBox>(GetWidgetFromName(TEXT("ChatScroll")));

}

void UIngameUserWidget::OnChangedText(const FText& Text)
{

}

void UIngameUserWidget::OnCommitedText(const FText& Text, ETextCommit::Type CommitMethod)
{
	switch (CommitMethod)
	{
	case ETextCommit::OnEnter:
	{
		ALobbyController* PC = Cast<ALobbyController>(GetOwningPlayer());
		if (PC)
		{
			UMyGameInstance* GI = Cast<UMyGameInstance>(UGameplayStatics::GetGameInstance(GetWorld()));
			if (GI)
			{
				FString Temp = FString::Printf(TEXT("%s : %s"), *GI->Username, *Text.ToString());

				//PC->C2S_SendMessage(FText::FromString(Temp));
			}
		}
	}
	}
}

void UIngameUserWidget::StartGame()
{
	GetWorld()->ServerTravel(TEXT("InGame"));
}
