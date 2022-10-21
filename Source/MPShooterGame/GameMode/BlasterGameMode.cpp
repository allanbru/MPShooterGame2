// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameMode.h"
#include "MPShooterGame/Character/BlasterCharacter.h"
#include "MPShooterGame/PlayerController/BlasterPlayerController.h"

void ABlasterGameMode::PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
	if (!ElimmedCharacter) return;
	ElimmedCharacter->Elim();
}
