// Fill out your copyright notice in the Description page of Project Settings.


#include "CaptureTheFlagGameMode.h"
#include "Kismet/GameplayStatics.h"
#include "MPShooterGame/CaptureTheFlag/FlagZone.h"
#include "MPShooterGame/GameState/BlasterGameState.h"
#include "MPShooterGame/Weapon/Flag.h"

void ACaptureTheFlagGameMode::PlayerEliminated(ABlasterCharacter* ElimmedCharacter, ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController)
{
	ABlasterGameMode::PlayerEliminated(ElimmedCharacter, VictimController, AttackerController);
}

bool ACaptureTheFlagGameMode::FlagCaptured(AFlag* Flag, AFlagZone* Zone)
{
	bool bValidCapture = Flag->GetTeam() != Zone->Team;
	if (!bValidCapture) return false;
	TArray<AActor*> AllFlags;
	UGameplayStatics::GetAllActorsOfClass(this, AFlag::StaticClass(), AllFlags);
	ABlasterGameState* BGameState = Cast<ABlasterGameState>(GameState);
	if (BGameState)
	{
		// Check if other team's flags are in base
		if (bCheckTeamFlagInBase && AllFlags.Num() > 0)
		{
			for (auto FlagInMap : AllFlags)
			{
				AFlag* FlagToCheck = Cast<AFlag>(FlagInMap);
				if (FlagToCheck && FlagToCheck->GetTeam() == Zone->Team &&!FlagToCheck->IsInBase()) return false;
			}
		}
		if (Zone->Team == ETeam::ET_Blue)
		{
			BGameState->BlueTeamScores();
			Flag->ResetFlag();
		}
		else if (Zone->Team == ETeam::ET_Red)
		{
			BGameState->RedTeamScores();
			Flag->ResetFlag();
		}
	}
	return bValidCapture;
}
