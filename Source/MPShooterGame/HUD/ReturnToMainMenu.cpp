// Fill out your copyright notice in the Description page of Project Settings.


#include "ReturnToMainMenu.h"
#include "Components/Button.h"
#include "GameFramework/GameModeBase.h"
#include "GameFramework/PlayerController.h"
#include "MultiplayerSessionsSubsystem.h"
#include "MPShooterGame/Character/BlasterCharacter.h"


bool UReturnToMainMenu::Initialize()
{
	if (!Super::Initialize())
	{
		return false;
	}

	return true;
}

void UReturnToMainMenu::MenuSetup()
{
	AddToViewport();
	SetVisibility(ESlateVisibility::Visible);
	bIsFocusable = true;

	PlayerController = (PlayerController == nullptr) ? GetOwningPlayer() : PlayerController;
	if (PlayerController)
	{
		FInputModeGameAndUI InputModeData;
		InputModeData.SetWidgetToFocus(TakeWidget());
		PlayerController->SetInputMode(InputModeData);
		PlayerController->SetShowMouseCursor(true);
	}

	if (ReturnButton && !ReturnButton->OnClicked.IsBound())
	{
		ReturnButton->OnClicked.AddDynamic(this, &UReturnToMainMenu::ReturnButtonClicked);
	}

	UGameInstance* GameInstance = GetGameInstance();
	if (GameInstance)
	{
		MultiplayerSessionsSubsystem = GameInstance->GetSubsystem<UMultiplayerSessionsSubsystem>();
		if (MultiplayerSessionsSubsystem && !MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.IsAlreadyBound(this, &UReturnToMainMenu::OnDestroySession))
		{
			MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.AddDynamic(this, &UReturnToMainMenu::OnDestroySession);
		}
	}
}

void UReturnToMainMenu::MenuTearDown()
{
	RemoveFromParent();
	PlayerController = (PlayerController == nullptr) ? GetOwningPlayer() : PlayerController;
	if (PlayerController)
	{
		FInputModeGameOnly InputModeData;
		PlayerController->SetInputMode(InputModeData);
		PlayerController->SetShowMouseCursor(false);
	}

	if (ReturnButton && ReturnButton->OnClicked.IsBound())
	{
		ReturnButton->OnClicked.RemoveDynamic(this, &UReturnToMainMenu::ReturnButtonClicked);
	}

	if (MultiplayerSessionsSubsystem && MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.IsBound())
	{
		MultiplayerSessionsSubsystem->MultiplayerOnDestroySessionComplete.RemoveDynamic(this, &UReturnToMainMenu::OnDestroySession);
	}
}

void UReturnToMainMenu::ReturnButtonClicked()
{
	ReturnButton->SetIsEnabled(false);
	
	UWorld* World = GetWorld();
	if (World)
	{
		APlayerController* FirstPlayerController = World->GetFirstPlayerController();
		if (FirstPlayerController)
		{
			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FirstPlayerController->GetPawn());
			if (BlasterCharacter)
			{
				BlasterCharacter->ServerLeaveGame();
				BlasterCharacter->OnLeftGame.AddDynamic(this, &UReturnToMainMenu::OnPlayerLeftGame);
			}
			else
			{
				ReturnButton->SetIsEnabled(true);
			}
		}
	}
}

void UReturnToMainMenu::OnPlayerLeftGame()
{
	if (MultiplayerSessionsSubsystem)
	{
		MultiplayerSessionsSubsystem->DestroySession();

	}
}

void UReturnToMainMenu::OnDestroySession(bool bWasSuccessful)
{
	if (!bWasSuccessful)
	{
		ReturnButton->SetIsEnabled(true);
		return;
	}

	UWorld* World = GetWorld();
	if (World)
	{
		AGameModeBase* GameMode = World->GetAuthGameMode<AGameModeBase>();
		if (GameMode)
		{
			// server
			GameMode->ReturnToMainMenuHost();
		}
		else
		{
			// client
			PlayerController = (PlayerController == nullptr) ? GetOwningPlayer() : PlayerController;
			if (PlayerController)
			{
				PlayerController->ClientReturnToMainMenuWithTextReason(FText());
			}
		}
	}
}
