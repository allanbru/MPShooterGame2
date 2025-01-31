// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterGameState.h"
#include "MPShooterGame/PlayerState/BlasterPlayerState.h"
#include "MPShooterGame/PlayerController/BlasterPlayerController.h"
#include "Net/UnrealNetwork.h"

ABlasterGameState::ABlasterGameState()
{
	TopScoringPlayers.Empty();
}

void ABlasterGameState::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterGameState, TopScoringPlayers);
	DOREPLIFETIME(ABlasterGameState, RedTeamScore);
	DOREPLIFETIME(ABlasterGameState, BlueTeamScore);
}

void ABlasterGameState::UpdateTopScore(ABlasterPlayerState* ScoringPlayer)
{
	if (TopScoringPlayers.Num() == 0) {
		TopScoringPlayers.Add(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	} 
	else if (ScoringPlayer->GetScore() == TopScore)
	{
		TopScoringPlayers.AddUnique(ScoringPlayer);
	}
	else if (ScoringPlayer->GetScore() > TopScore)
	{
		TopScoringPlayers.Empty();
		TopScoringPlayers.AddUnique(ScoringPlayer);
		TopScore = ScoringPlayer->GetScore();
	}
}

void ABlasterGameState::RedTeamScores()
{
	++RedTeamScore;

	ABlasterPlayerController* BPlayerController = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayerController)
	{
		BPlayerController->SetHUDRedTeamScore(RedTeamScore);
	}
}

void ABlasterGameState::BlueTeamScores()
{
	++BlueTeamScore;

	ABlasterPlayerController* BPlayerController = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayerController)
	{
		BPlayerController->SetHUDBlueTeamScore(BlueTeamScore);
	}
}

void ABlasterGameState::OnRep_RedTeamScore()
{
	ABlasterPlayerController* BPlayerController = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayerController)
	{
		BPlayerController->SetHUDRedTeamScore(RedTeamScore);
	}
}

void ABlasterGameState::OnRep_BlueTeamScore()
{
	ABlasterPlayerController* BPlayerController = Cast<ABlasterPlayerController>(GetWorld()->GetFirstPlayerController());
	if (BPlayerController)
	{
		BPlayerController->SetHUDBlueTeamScore(BlueTeamScore);
	}
}
