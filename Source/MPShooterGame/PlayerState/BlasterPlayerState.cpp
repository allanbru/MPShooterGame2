// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerState.h"
#include "MPShooterGame/Character/BlasterCharacter.h"
#include "MPShooterGame/PlayerController/BlasterPlayerController.h"
#include "Net/UnrealNetwork.h"

void ABlasterPlayerState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerState, Defeats);
}

void ABlasterPlayerState::AddToScore(float ScoreAmount)
{
	SetScore(Score + ScoreAmount);
	Character = (Character == nullptr) ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (!Character) return;

	Controller = (Controller == nullptr) ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (!Controller) return;

	Controller->SetHUDScore(Score);
}

void ABlasterPlayerState::OnRep_Score()
{
	Super::OnRep_Score();

	Character = (Character == nullptr) ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (!Character) return;

	Controller = (Controller == nullptr) ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (!Controller) return;

	Controller->SetHUDScore(GetScore());
	
}

void ABlasterPlayerState::AddToDefeats(int32 DefeatsAmount)
{
	Defeats += DefeatsAmount;
	Character = (Character == nullptr) ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (!Character) return;

	Controller = (Controller == nullptr) ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (!Controller) return;

	Controller->SetHUDDefeats(Defeats);
}

void ABlasterPlayerState::OnRep_Defeats()
{
	Character = (Character == nullptr) ? Cast<ABlasterCharacter>(GetPawn()) : Character;
	if (!Character) return;

	Controller = (Controller == nullptr) ? Cast<ABlasterPlayerController>(Character->Controller) : Controller;
	if (!Controller) return;

	Controller->SetHUDDefeats(Defeats);
}

