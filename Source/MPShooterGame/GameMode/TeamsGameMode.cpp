// Fill out your copyright notice in the Description page of Project Settings.


#include "TeamsGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "MPShooterGame/GameState/BlasterGameState.h"
#include "MPShooterGame/PlayerState/BlasterPlayerState.h"
#include "MPShooterGame/PlayerController/BlasterPlayerController.h"

ATeamsGameMode::ATeamsGameMode()
{
	bTeamsMatch = true;
}

void ATeamsGameMode::PostLogin(APlayerController* NewPlayer)
{
	Super::PostLogin(NewPlayer);
	ABlasterGameState* BGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
	ABlasterPlayerState* BPState = NewPlayer->GetPlayerState<ABlasterPlayerState>();
	if (BGameState && BPState && BPState->GetTeam() == ETeam::ET_NoTeam)
	{
		if (BGameState->BlueTeam.Num() >= BGameState->RedTeam.Num())
		{
			BGameState->RedTeam.AddUnique(BPState);
			BPState->SetTeam(ETeam::ET_Red);
		}
		else
		{
			BGameState->BlueTeam.AddUnique(BPState);
			BPState->SetTeam(ETeam::ET_Blue);
		}
	}
}

void ATeamsGameMode::Logout(AController* Exiting)
{
	
	ABlasterGameState* BGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
	ABlasterPlayerState* BPState = Exiting->GetPlayerState<ABlasterPlayerState>();
	if (BGameState && BPState)
	{
		if (BGameState->RedTeam.Contains(BPState))
		{
			BGameState->RedTeam.Remove(BPState);
		} 
		else if (BGameState->BlueTeam.Contains(BPState))
		{
			BGameState->BlueTeam.Remove(BPState);
		}
	}
	Super::Logout(Exiting);
}

void ATeamsGameMode::HandleMatchHasStarted()
{
	Super::HandleMatchHasStarted();

	ABlasterGameState* BGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
	if (BGameState)
	{
		for (auto PState : BGameState->PlayerArray)
		{
			ABlasterPlayerState* BPState = Cast<ABlasterPlayerState>(PState.Get());
			if (BPState && BPState->GetTeam() == ETeam::ET_NoTeam)
			{
				if (BGameState->BlueTeam.Num() >= BGameState->RedTeam.Num())
				{
					BGameState->RedTeam.AddUnique(BPState);
					BPState->SetTeam(ETeam::ET_Red);
				}
				else
				{
					BGameState->BlueTeam.AddUnique(BPState);
					BPState->SetTeam(ETeam::ET_Blue);
				}
			}
		}
	}

}

float ATeamsGameMode::CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage)
{
	if (Attacker == nullptr || Victim == nullptr) return BaseDamage;
	ABlasterPlayerState* AttackerPState = Attacker->GetPlayerState<ABlasterPlayerState>();
	ABlasterPlayerState* VictimPState = Victim->GetPlayerState<ABlasterPlayerState>();
	if (AttackerPState == nullptr || VictimPState == nullptr) return BaseDamage;
	if (AttackerPState == VictimPState) return BaseDamage;
	if (AttackerPState->GetTeam() == VictimPState->GetTeam())
	{
		return (bAllowFriendlyFire) ? BaseDamage : 0.f;
	}
	return BaseDamage;
}

void ATeamsGameMode::PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
	Super::PlayerEliminated(ElimmedCharacter, VictimController, AttackerController);

	ABlasterGameState* BGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
	ABlasterPlayerState* AttackerPlayerState = (AttackerController) ? Cast<ABlasterPlayerState>(AttackerController->PlayerState) : nullptr;
	ABlasterPlayerState* VictimPlayerState = (VictimController) ? Cast<ABlasterPlayerState>(VictimController->PlayerState) : nullptr;
	if (BGameState && AttackerPlayerState && VictimPlayerState && AttackerPlayerState->GetTeam() != VictimPlayerState->GetTeam())
	{
		if (AttackerPlayerState->GetTeam() == ETeam::ET_Blue)
		{
			BGameState->BlueTeamScores();
		}
		else if (AttackerPlayerState->GetTeam() == ETeam::ET_Red)
		{
			BGameState->RedTeamScores();
		}
	}

}