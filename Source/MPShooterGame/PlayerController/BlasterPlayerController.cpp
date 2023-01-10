// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"
#include "Components/Image.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "MPShooterGame/BlasterComponents/CombatComponent.h"
#include "MPShooterGame/BlasterTypes/Announcement.h"
#include "MPShooterGame/Character/BlasterCharacter.h"
#include "MPShooterGame/GameMode/BlasterGameMode.h"
#include "MPShooterGame/GameState/BlasterGameState.h"
#include "MPShooterGame/PlayerState/BlasterPlayerState.h"
#include "MPShooterGame/HUD/Announcement.h"
#include "MPShooterGame/HUD/BlasterHUD.h"
#include "MPShooterGame/HUD/CharacterOverlay.h"
#include "MPShooterGame/HUD/ReturnToMainMenu.h"
#include "MPShooterGame/Weapon/Weapon.h"
#include "Net/UnrealNetwork.h"

void ABlasterPlayerController::BeginPlay()
{
	Super::BeginPlay();

	BlasterHUD = Cast<ABlasterHUD>(GetHUD());

	ServerCheckMatchState();
}

void ABlasterPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();
	if (InputComponent == nullptr) return;
	// ClassSelection
	InputComponent->BindAction(TEXT("ChangeClass"), EInputEvent::IE_Pressed, this, &ABlasterPlayerController::SetHUDClassChange);
	// PauseMenu
	InputComponent->BindAction(TEXT("Quit"), EInputEvent::IE_Pressed, this, &ABlasterPlayerController::ShowReturnToMainMenu);
}

void ABlasterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerController, MatchState);
	DOREPLIFETIME(ABlasterPlayerController, bShowTeamScores);
}


void ABlasterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();
	CheckTimeSync(DeltaTime);
	PollInit();
	CheckPing(DeltaTime);
}

void ABlasterPlayerController::CheckTimeSync(float DeltaTime)
{
	TimeSyncRunningTime += DeltaTime;
	if (IsLocalController() && TimeSyncRunningTime >= TimeSyncFrequency)
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
		TimeSyncRunningTime = 0.f;
	}
}

void ABlasterPlayerController::PollInit()
{
	if (CharacterOverlay == nullptr)
	{
		if (BlasterHUD && BlasterHUD->CharacterOverlay)
		{
			CharacterOverlay = BlasterHUD->CharacterOverlay;
			if (CharacterOverlay)
			{
				if (bInitializeHealth) SetHUDHealth(HUDHealth, HUDMaxHealth);
				if (bInitializeShield) SetHUDShield(HUDShield, HUDMaxShield);
				if (bInitializeScore) SetHUDScore(HUDScore);
				if (bInitializeDefeats) SetHUDDefeats(HUDDefeats);
				if (bInitializeWeaponAmmo) SetHUDWeaponAmmo(HUDWeaponAmmo);
				if (bInitializeCarriedAmmo) SetHUDCarriedAmmo(HUDCarriedAmmo);
				ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
				if (BlasterCharacter)
				{	
					if(bInitializeGrenades&& BlasterCharacter->GetCombat()) {
						SetHUDGrenades(BlasterCharacter->GetCombat()->GetGrenades());
					}
					if (BlasterCharacter->GetEquippedWeapon() == nullptr)
					{
						BlasterCharacter->SetStartingWeaponClass(StartingWeaponClass);
						BlasterCharacter->ServerEquipStartingWeapon();
					}
				}
			}
		}
	}
}

void ABlasterPlayerController::CheckPing(float DeltaTime)
{
	HighPingRunningTime += DeltaTime;
	if (HighPingRunningTime > CheckPingFrequency)
	{
		PlayerState = (PlayerState == nullptr) ? GetPlayerState<APlayerState>() : PlayerState;
		if (PlayerState)
		{
			//Due to compression, the real ping is PlayerState->GetPing() * 4
			if (PlayerState->GetCompressedPing() * 4 > HighPingThreshold)
			{
				HighPingWarning();
				PingAnimationRunningTime = 0.f;
				ServerReportPingStatus(true);
			}
			else
			{
				ServerReportPingStatus(false);
			}
		}
		HighPingRunningTime = 0.f;
	}
	bool bHighPingAnimationPlaying =
		BlasterHUD && BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->HighPingAnimation &&
		BlasterHUD->CharacterOverlay->IsAnimationPlaying(BlasterHUD->CharacterOverlay->HighPingAnimation);
	if (bHighPingAnimationPlaying)
	{
		PingAnimationRunningTime += DeltaTime;
		if (PingAnimationRunningTime > HighPingDuration)
		{
			StopHighPingWarning();
		}
	}
}

//Is the ping too high?
void ABlasterPlayerController::ServerReportPingStatus_Implementation(bool bHighPing)
{
	HighPingDelegate.Broadcast(bHighPing);
}

void ABlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn);
	if (BlasterCharacter)
	{
		SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
		SetHUDShield(BlasterCharacter->GetShield(), BlasterCharacter->GetMaxShield());
		BlasterCharacter->SetStartingWeaponClass(StartingWeaponClass);
		BlasterCharacter->ServerEquipStartingWeapon();
		BlasterCharacter->UpdateHUDAmmo();
	}
}

void ABlasterPlayerController::HighPingWarning()
{
	BlasterHUD = (BlasterHUD == nullptr) ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->HighPingImage &&
		BlasterHUD->CharacterOverlay->HighPingAnimation;
	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->HighPingImage->SetOpacity(1.f);
		BlasterHUD->CharacterOverlay->PlayAnimation(BlasterHUD->CharacterOverlay->HighPingAnimation, 0.f, 5);
	}
}

void ABlasterPlayerController::StopHighPingWarning()
{
	BlasterHUD = (BlasterHUD == nullptr) ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->HighPingImage &&
		BlasterHUD->CharacterOverlay->HighPingAnimation;
	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->HighPingImage->SetOpacity(0.f);
		if (BlasterHUD->CharacterOverlay->IsAnimationPlaying(BlasterHUD->CharacterOverlay->HighPingAnimation)) {
			BlasterHUD->CharacterOverlay->StopAnimation(BlasterHUD->CharacterOverlay->HighPingAnimation);
		}
	}
}

void ABlasterPlayerController::ServerCheckMatchState_Implementation()
{
	ABlasterGameMode* GameMode = Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this));
	if (GameMode)
	{
		WarmupTime = GameMode->WarmupTime;
		MatchTime = GameMode->MatchTime;
		CooldownTime = GameMode->CooldownTime;
		LevelStartingTime = GameMode->LevelStartingTime;
		MatchState = GameMode->GetMatchState();
		bShowTeamScores = GameMode->bTeamsMatch;

		ClientJoinMidgame(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartingTime, bShowTeamScores);
	}
}

void ABlasterPlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime, bool bTeamsMatch)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;
	bShowTeamScores = bTeamsMatch;

	OnMatchStateSet(MatchState);
	if (BlasterHUD && MatchState == MatchState::WaitingToStart)
	{
		BlasterHUD->AddAnnouncement();
	}
}


void ABlasterPlayerController::ShowReturnToMainMenu()
{
	if (ReturnToMainMenuWidget == nullptr) return;
	if (ReturnToMainMenu == nullptr)
	{
		ReturnToMainMenu = CreateWidget<UReturnToMainMenu>(this, ReturnToMainMenuWidget);
	}
	if (ReturnToMainMenu)
	{
		bReturnToMainMenuOpen = !bReturnToMainMenuOpen;
		if (bReturnToMainMenuOpen)
		{
			ReturnToMainMenu->MenuSetup();
		}
		else
		{
			ReturnToMainMenu->MenuTearDown();
		}
	}

}

void ABlasterPlayerController::SetHUDHealth(float Health, float MaxHealth)
{
	BlasterHUD = (BlasterHUD == nullptr) ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->HealthBar &&
		BlasterHUD->CharacterOverlay->HealthText;
	if (!bHUDValid)
	{
		bInitializeHealth = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
		return;
	}

	const float HealthPercent = Health / MaxHealth;
	BlasterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
	FString	HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
	BlasterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
}

void ABlasterPlayerController::SetHUDShield(float Shield, float MaxShield)
{
	BlasterHUD = (BlasterHUD == nullptr) ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->ShieldBar &&
		BlasterHUD->CharacterOverlay->ShieldText;
	if (!bHUDValid)
	{
		bInitializeShield = true;
		HUDShield = Shield;
		HUDMaxShield = MaxShield;
		return;
	}

	const float ShieldPercent = Shield / MaxShield;
	BlasterHUD->CharacterOverlay->ShieldBar->SetPercent(ShieldPercent);
	FString	ShieldText = FString::Printf(TEXT("%d"), FMath::CeilToInt(Shield));
	BlasterHUD->CharacterOverlay->ShieldText->SetText(FText::FromString(ShieldText));
}

void ABlasterPlayerController::SetHUDScore(float Score)
{
	BlasterHUD = (BlasterHUD == nullptr) ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->ScoreAmount;
	if (!bHUDValid)
	{
		bInitializeScore = true;
		HUDScore = Score;
		return;
	}
	FString	ScoreText = FString::Printf(TEXT("%d"), FMath::CeilToInt(Score));
	BlasterHUD->CharacterOverlay->ScoreAmount->SetText(FText::FromString(ScoreText));
}

void ABlasterPlayerController::SetHUDDefeats(int32 Defeats)
{
	BlasterHUD = (BlasterHUD == nullptr) ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->DefeatsAmount;
	if (!bHUDValid) {
		bInitializeDefeats = true;
		HUDDefeats = Defeats;
		return;
	}
	FString	DefeatsText = FString::Printf(TEXT("%d"), Defeats);
	BlasterHUD->CharacterOverlay->DefeatsAmount->SetText(FText::FromString(DefeatsText));
}

void ABlasterPlayerController::SetHUDWeaponAmmo(int32 Ammo)
{
	BlasterHUD = (BlasterHUD == nullptr) ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->WeaponAmmoAmount;
	if (!bHUDValid) {
		bInitializeWeaponAmmo = true;
		HUDWeaponAmmo = Ammo;
		return;
	}
	FString	AmmoText = FString::Printf(TEXT("%d"), Ammo);
	BlasterHUD->CharacterOverlay->WeaponAmmoAmount->SetText(FText::FromString(AmmoText));
}

void ABlasterPlayerController::SetHUDCarriedAmmo(int32 Ammo)
{
	BlasterHUD = (BlasterHUD == nullptr) ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->CarriedAmmoAmount;
	if (!bHUDValid) {
		bInitializeCarriedAmmo = true;
		HUDCarriedAmmo = Ammo;
		return;
	}
	FString	AmmoText = FString::Printf(TEXT("%d"), Ammo);
	BlasterHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
}

void ABlasterPlayerController::SetHUDGrenades(int32 Grenades)
{
	BlasterHUD = (BlasterHUD == nullptr) ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->GrenadesAmount;
	if (!bHUDValid) {
		bInitializeGrenades = true;
		HUDGrenades = Grenades;
		return;
	}
	FString	GrenadesText = FString::Printf(TEXT("%d"), Grenades);
	BlasterHUD->CharacterOverlay->GrenadesAmount->SetText(FText::FromString(GrenadesText));
}

void ABlasterPlayerController::SetHUDMatchCountdown(float CountdownTime)
{
	BlasterHUD = (BlasterHUD == nullptr) ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->MatchCountdownText;
	if (!bHUDValid) return;
	if (CountdownTime < 0.f)
	{
		BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText());
		return;
	}
	int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
	int32 Seconds = CountdownTime - Minutes * 60;

	FString	CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	BlasterHUD->CharacterOverlay->MatchCountdownText->SetText(FText::FromString(CountdownText));
}

void ABlasterPlayerController::SetHUDAnnouncementCountdown(float CountdownTime)
{
	BlasterHUD = (BlasterHUD == nullptr) ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->Announcement &&
		BlasterHUD->Announcement->WarmupTime;
	if (!bHUDValid) return;
	if (CountdownTime < 0.f)
	{
		BlasterHUD->Announcement->WarmupTime->SetText(FText());
		return;
	}
	int32 Minutes = FMath::FloorToInt(CountdownTime / 60.f);
	int32 Seconds = CountdownTime - Minutes * 60;

	FString	CountdownText = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
	BlasterHUD->Announcement->WarmupTime->SetText(FText::FromString(CountdownText));
}

void ABlasterPlayerController::SetHUDClassChange()
{
	BlasterHUD = (BlasterHUD == nullptr) ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
		
		bShowMouseCursor = !BlasterHUD->bClassSelectorOnScreen;
		
		if (bShowMouseCursor) 
		{
			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
			if (BlasterCharacter && BlasterCharacter->GetCombat()) {
				SetInputMode(FInputModeUIOnly());
				BlasterCharacter->GetCombat()->FireButtonPressed(false);
			}
		}
		else
		{
			SetInputMode(FInputModeGameOnly());
		}

		BlasterHUD->ToggleClassSelectorWidget();
		
	}
}

void ABlasterPlayerController::SetHUDTime()
{
	float TimeLeft = 0.f;
	
	if (MatchState == MatchState::WaitingToStart)
	{
		TimeLeft = WarmupTime - GetServerTime() + LevelStartingTime;
	}
	else if (MatchState == MatchState::InProgress)
	{
		TimeLeft = WarmupTime + MatchTime - GetServerTime() + LevelStartingTime;
	}
	else if (MatchState == MatchState::Cooldown)
	{
		TimeLeft = WarmupTime + MatchTime + CooldownTime - GetServerTime() + LevelStartingTime;
	}
	
	if (HasAuthority())
	{
		BlasterGameMode = BlasterGameMode == nullptr ? Cast<ABlasterGameMode>(UGameplayStatics::GetGameMode(this)) : BlasterGameMode;
		if (BlasterGameMode)
		{
			TimeLeft = BlasterGameMode->GetCountdownTime() + LevelStartingTime;
		}
	}
	
	uint32 SecondsLeft = FMath::CeilToInt(TimeLeft);	
	
	if (CountdownInt != SecondsLeft) {
		if (MatchState == MatchState::WaitingToStart || MatchState == MatchState::Cooldown)
		{
			SetHUDAnnouncementCountdown(TimeLeft);
		}
		else if (MatchState == MatchState::InProgress)
		{
			SetHUDMatchCountdown(TimeLeft);
		}
	}
	CountdownInt = SecondsLeft;
}

void ABlasterPlayerController::ServerRequestServerTime_Implementation(float TimeOfClientRequest)
{
	float ServerTimeOfReceipt = GetWorld()->GetTimeSeconds();
	ClientReportServerTime(TimeOfClientRequest, ServerTimeOfReceipt);
}

void ABlasterPlayerController::ClientReportServerTime_Implementation(float TimeOfClientRequest, float TimeServerReceivedClientRequest)
{
	float RoundTripTime = GetWorld()->GetTimeSeconds() - TimeOfClientRequest;
	SingleTripTime = .5f * RoundTripTime;
	float CurrentServerTime = TimeServerReceivedClientRequest + SingleTripTime;
	ClientServerDelta = CurrentServerTime - GetWorld()->GetTimeSeconds();
}

float ABlasterPlayerController::GetServerTime()
{
	if (HasAuthority()) return GetWorld()->GetTimeSeconds();
	return GetWorld()->GetTimeSeconds() + ClientServerDelta;
}

void ABlasterPlayerController::ReceivedPlayer()
{
	Super::ReceivedPlayer();
	if (IsLocalController()) 
	{
		ServerRequestServerTime(GetWorld()->GetTimeSeconds());
	}
}

void ABlasterPlayerController::OnMatchStateSet(FName State, bool bTeamsMatch)
{
	MatchState = State;

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted(bTeamsMatch);
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ABlasterPlayerController::OnRep_MatchState()
{
	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
	}
	else if (MatchState == MatchState::Cooldown)
	{
		HandleCooldown();
	}
}

void ABlasterPlayerController::HandleMatchHasStarted(bool bTeamsMatch)
{
	if(HasAuthority()) bShowTeamScores = bTeamsMatch;
	BlasterHUD = (BlasterHUD == nullptr) ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
		if(BlasterHUD->CharacterOverlay == nullptr) BlasterHUD->AddCharacterOverlay();
		if (BlasterHUD->Announcement)
		{
			BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
		}
		if (HasAuthority())
		{
			if (bTeamsMatch)
			{
				InitTeamScores();
			}
			else
			{
				HideTeamScores();
			}
		}
	}
}

void ABlasterPlayerController::HandleCooldown()
{
	BlasterHUD = (BlasterHUD == nullptr) ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
		BlasterHUD->CharacterOverlay->RemoveFromParent();
		bool bHUDValid = BlasterHUD->Announcement &&
			BlasterHUD->Announcement->AnnouncementText &&
			BlasterHUD->Announcement->InfoText;
		if (bHUDValid)
		{
			BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Visible);
			FString AnnouncementText = *Announcement::NewMatchStartsIn;
			BlasterHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));

			ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
			ABlasterPlayerState* BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
			if (BlasterGameState && BlasterPlayerState)
			{
				TArray<ABlasterPlayerState*> TopPlayers = BlasterGameState->TopScoringPlayers;
				FString InfoTextString = (bShowTeamScores) ? GetTeamsInfoText(BlasterGameState) : GetInfoText(TopPlayers);
				
				BlasterHUD->Announcement->InfoText->SetText(FText::FromString(InfoTextString));
			}
			

		}
	}

	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
	if (BlasterCharacter && BlasterCharacter->GetCombat()) {
		BlasterCharacter->bDisableGameplay = true;
		BlasterCharacter->GetCombat()->FireButtonPressed(false);
	}
}

FString ABlasterPlayerController::GetInfoText(const TArray<class ABlasterPlayerState*>& Players)
{
	ABlasterPlayerState* BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
	if (BlasterPlayerState == nullptr) return FString();
	FString InfoTextString;
	if (Players.Num() == 0)
	{
		InfoTextString = *Announcement::ThereIsNoWinner;
		InfoTextString.Append("\nOMEGALUL");
	}
	else if (Players.Num() == 1)
	{
		if (Players[0] == BlasterPlayerState)
		{
			InfoTextString = *Announcement::YouWin;
		}
		else
		{
			InfoTextString = FString::Printf(TEXT("Winner: \n%s\nBetter luck next time..."), *Players[0]->GetPlayerName());
		}
	}
	else if (Players.Num() > 1)
	{
		bool bWon = false;
		InfoTextString = *Announcement::PlayersTiedForTheWin;
		InfoTextString.Append("\n");
		for (auto TiedPlayer : Players)
		{
			InfoTextString.Append(FString::Printf(TEXT("%s\n"), *TiedPlayer->GetPlayerName()));
			if (TiedPlayer->GetPlayerName() == BlasterPlayerState->GetPlayerName()) bWon = true;
		}
		if (bWon) {
			InfoTextString.Append(FString::Printf(TEXT("Congratulations!")));
		}
		else
		{
			InfoTextString.Append(FString::Printf(TEXT("Better luck next time...")));
		}
	}
	return InfoTextString;
}

FString ABlasterPlayerController::GetTeamsInfoText(ABlasterGameState* BlasterGameState)
{
	if(BlasterGameState == nullptr) return FString();
	FString InfoTextString;

	const uint32 RedTeamScore = FMath::Floor<uint32>(BlasterGameState->RedTeamScore);
	const uint32 BlueTeamScore = FMath::Floor<uint32>(BlasterGameState->BlueTeamScore);

	if (RedTeamScore == 0 && BlueTeamScore == 0)
	{
		InfoTextString = *Announcement::ThereIsNoWinner;
	}
	else if (RedTeamScore == BlueTeamScore)
	{
		InfoTextString = FString::Printf(TEXT("%s\n"), *Announcement::TeamsTiedForTheWin);
		InfoTextString.Append(*Announcement::BlueTeam);
		InfoTextString.Append(TEXT(" handshakes "));
		InfoTextString.Append(*Announcement::RedTeam);
	}
	else if (RedTeamScore > BlueTeamScore)
	{
		InfoTextString = *Announcement::RedTeamWins;
		InfoTextString.Append(FString::Printf(TEXT("%d - %d"), RedTeamScore, BlueTeamScore));
		if (RedTeamScore >= 2 * BlueTeamScore)
		{
			InfoTextString.Append(TEXT("\nGG ez"));
		}
	}
	else if (BlueTeamScore > RedTeamScore)
	{
		InfoTextString = *Announcement::BlueTeamWins;
		InfoTextString.Append(FString::Printf(TEXT("\n%d - %d"), BlueTeamScore, RedTeamScore));
		if (BlueTeamScore >= 2 * RedTeamScore)
		{
			InfoTextString.Append(TEXT("\nGG ez"));
		}
	}

	return InfoTextString;
}

void ABlasterPlayerController::SetStartingWeaponClass(TSubclassOf<AWeapon> WeaponClass)
{
	StartingWeaponClass = WeaponClass;
	if(!HasAuthority()) ServerSetStartingWeaponClass(WeaponClass);
}

void ABlasterPlayerController::ServerSetStartingWeaponClass_Implementation(TSubclassOf<AWeapon> WeaponClass)
{
	StartingWeaponClass = WeaponClass;
}

void ABlasterPlayerController::BroadcastElim(APlayerState* Attacker, APlayerState* Victim)
{
	ClientElimAnnouncement(Attacker, Victim);
}

void ABlasterPlayerController::ClientElimAnnouncement_Implementation(APlayerState* Attacker, APlayerState* Victim)
{
	APlayerState* Self = GetPlayerState<APlayerState>();
	if (Attacker && Victim && Self)
	{
		BlasterHUD = (BlasterHUD == nullptr) ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
		if (BlasterHUD)
		{
			if (Attacker == Self && Victim != Self)
			{
				BlasterHUD->AddElimAnnouncement("You", Victim->GetPlayerName());
				return;
			}
			if (Victim == Self && Attacker != Self)
			{
				BlasterHUD->AddElimAnnouncement(Attacker->GetPlayerName(), "you");
				return;
			}
			if (Attacker == Victim)
			{
				if (Attacker == Self)
				{
					BlasterHUD->AddElimAnnouncement("You", "yourself");
					return;
				}
				BlasterHUD->AddElimAnnouncement(Attacker->GetPlayerName(), "themselves");
				return;
			}
			BlasterHUD->AddElimAnnouncement(Attacker->GetPlayerName(), Victim->GetPlayerName());
			return;
		}
	}
}

void ABlasterPlayerController::HideTeamScores()
{
	BlasterHUD = (BlasterHUD == nullptr) ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->RedTeamScore &&
		BlasterHUD->CharacterOverlay->BlueTeamScore &&
		BlasterHUD->CharacterOverlay->ScoreSpacerText;
	if (bHUDValid)
	{
		BlasterHUD->CharacterOverlay->RedTeamScore->SetText(FText());
		BlasterHUD->CharacterOverlay->BlueTeamScore->SetText(FText());
		BlasterHUD->CharacterOverlay->ScoreSpacerText->SetText(FText());
	}
}

void ABlasterPlayerController::InitTeamScores()
{
	BlasterHUD = (BlasterHUD == nullptr) ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->RedTeamScore &&
		BlasterHUD->CharacterOverlay->BlueTeamScore &&
		BlasterHUD->CharacterOverlay->ScoreSpacerText;
	if (bHUDValid)
	{
		FString Zero("0");
		FString Spacer(":");
		BlasterHUD->CharacterOverlay->RedTeamScore->SetText(FText::FromString(Zero));
		BlasterHUD->CharacterOverlay->BlueTeamScore->SetText(FText::FromString(Zero));
		BlasterHUD->CharacterOverlay->ScoreSpacerText->SetText(FText::FromString(Spacer));
	}
}

void ABlasterPlayerController::SetHUDRedTeamScore(float Score)
{
	BlasterHUD = (BlasterHUD == nullptr) ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->RedTeamScore;
	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::Floor<uint32>(Score));
		BlasterHUD->CharacterOverlay->RedTeamScore->SetText(FText::FromString(ScoreText));
	}
}

void ABlasterPlayerController::SetHUDBlueTeamScore(float Score)
{
	BlasterHUD = (BlasterHUD == nullptr) ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->BlueTeamScore;
	if (bHUDValid)
	{
		FString ScoreText = FString::Printf(TEXT("%d"), FMath::Floor<uint32>(Score));
		BlasterHUD->CharacterOverlay->BlueTeamScore->SetText(FText::FromString(ScoreText));
	}
}


void ABlasterPlayerController::OnRep_ShowTeamScores()
{
	if (bShowTeamScores)
	{
		InitTeamScores();
	}
	else
	{
		HideTeamScores();
	}
}