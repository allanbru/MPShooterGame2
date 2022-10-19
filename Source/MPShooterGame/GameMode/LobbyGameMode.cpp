// Fill out your copyright notice in the Description page of Project Settings.


#include "LobbyGameMode.h"
#include "GameFramework/GameStateBase.h"

void ALobbyGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	if (GEngine) {
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Login was successful!"));
	}
		
	int32 NumberOfPlayers = GameState.Get()->PlayerArray.Num();
	if (NumberOfPlayers == 2) {
		UWorld* World = GetWorld();
		if (World) {
			bUseSeamlessTravel = true;
			if (GEngine) {
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("2222222jkab ahje gfaehfbaukjLogin was successful!"));
			}
			World->ServerTravel(FString("/Game/Maps/BlasterMap?listen"));
		}
		else {
			if (GEngine) {
				GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, TEXT("Man, there is no fucking world here!!"));
			}
		}
	}
	else {
		if (GEngine) {
			GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Yellow, FString::Printf(TEXT("Less than 2p? %d"), NumberOfPlayers));
		}
	}
}
