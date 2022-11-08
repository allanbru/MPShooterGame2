// Fill out your copyright notice in the Description page of Project Settings.


#include "BlasterPlayerController.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Kismet/GameplayStatics.h"
#include "MPShooterGame/BlasterComponents/CombatComponent.h"
#include "MPShooterGame/Character/BlasterCharacter.h"
#include "MPShooterGame/GameMode/BlasterGameMode.h"
#include "MPShooterGame/GameState/BlasterGameState.h"
#include "MPShooterGame/PlayerState/BlasterPlayerState.h"
#include "MPShooterGame/HUD/Announcement.h"
#include "MPShooterGame/HUD/BlasterHUD.h"
#include "MPShooterGame/HUD/CharacterOverlay.h"
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
	//ClassSelection
	InputComponent->BindAction(TEXT("ChangeClass"), EInputEvent::IE_Pressed, this, &ABlasterPlayerController::SetHUDClassChange);
}

void ABlasterPlayerController::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	DOREPLIFETIME(ABlasterPlayerController, MatchState);
}


void ABlasterPlayerController::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	SetHUDTime();

	CheckTimeSync(DeltaTime);
	PollInit();
}

void ABlasterPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(InPawn);
	if (BlasterCharacter)
	{
		SetHUDHealth(BlasterCharacter->GetHealth(), BlasterCharacter->GetMaxHealth());
		BlasterCharacter->SetStartingWeaponClass(StartingWeaponClass);
		BlasterCharacter->ServerEquipStartingWeapon();
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
				SetHUDHealth(HUDHealth, HUDMaxHealth);
				SetHUDScore(HUDScore);
				SetHUDDefeats(HUDDefeats);
				SetHUDWeaponAmmo(HUDWeaponAmmo);
				SetHUDCarriedAmmo(HUDCarriedAmmo);
				ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(GetPawn());
				if (BlasterCharacter && !BlasterCharacter->IsWeaponEquipped()) {
					BlasterCharacter->SetStartingWeaponClass(StartingWeaponClass);
					BlasterCharacter->ServerEquipStartingWeapon();
				}
			}
		}
	}
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
		ClientJoinMidgame(MatchState, WarmupTime, MatchTime, CooldownTime, LevelStartingTime);
	}
}

void ABlasterPlayerController::ClientJoinMidgame_Implementation(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime)
{
	WarmupTime = Warmup;
	MatchTime = Match;
	CooldownTime = Cooldown;
	LevelStartingTime = StartingTime;
	MatchState = StateOfMatch;
	OnMatchStateSet(MatchState);
	if (BlasterHUD && MatchState == MatchState::WaitingToStart)
	{
		BlasterHUD->AddAnnouncement();
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
		bInitializeCharacterOverlay = true;
		HUDHealth = Health;
		HUDMaxHealth = MaxHealth;
		return;
	}

	const float HealthPercent = Health / MaxHealth;
	BlasterHUD->CharacterOverlay->HealthBar->SetPercent(HealthPercent);
	FString	HealthText = FString::Printf(TEXT("%d/%d"), FMath::CeilToInt(Health), FMath::CeilToInt(MaxHealth));
	BlasterHUD->CharacterOverlay->HealthText->SetText(FText::FromString(HealthText));
}

void ABlasterPlayerController::SetHUDScore(float Score)
{
	BlasterHUD = (BlasterHUD == nullptr) ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	bool bHUDValid = BlasterHUD &&
		BlasterHUD->CharacterOverlay &&
		BlasterHUD->CharacterOverlay->ScoreAmount;
	if (!bHUDValid)
	{
		bInitializeCharacterOverlay = true;
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
		bInitializeCharacterOverlay = true;
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
		bInitializeCharacterOverlay = true;
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
		bInitializeCharacterOverlay = true;
		HUDCarriedAmmo = Ammo;
		return;
	}
	FString	AmmoText = FString::Printf(TEXT("%d"), Ammo);
	BlasterHUD->CharacterOverlay->CarriedAmmoAmount->SetText(FText::FromString(AmmoText));
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
	
	if (!HasAuthority() || !BlasterGameMode)
	{
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
	}
	else
	{
		TimeLeft = BlasterGameMode->GetCountdownTime();
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
	float CurrentServerTime = TimeServerReceivedClientRequest - .5f * RoundTripTime;
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

void ABlasterPlayerController::OnMatchStateSet(FName State)
{
	MatchState = State;

	if (MatchState == MatchState::InProgress)
	{
		HandleMatchHasStarted();
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

void ABlasterPlayerController::HandleMatchHasStarted()
{
	BlasterHUD = (BlasterHUD == nullptr) ? Cast<ABlasterHUD>(GetHUD()) : BlasterHUD;
	if (BlasterHUD)
	{
		if(BlasterHUD->CharacterOverlay == nullptr) BlasterHUD->AddCharacterOverlay();
		if (BlasterHUD->Announcement)
		{
			BlasterHUD->Announcement->SetVisibility(ESlateVisibility::Hidden);
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
			FString AnnouncementText("New match starts in:");
			BlasterHUD->Announcement->AnnouncementText->SetText(FText::FromString(AnnouncementText));

			ABlasterGameState* BlasterGameState = Cast<ABlasterGameState>(UGameplayStatics::GetGameState(this));
			ABlasterPlayerState* BlasterPlayerState = GetPlayerState<ABlasterPlayerState>();
			if (BlasterGameState && BlasterPlayerState)
			{
				TArray<ABlasterPlayerState*> TopPlayers = BlasterGameState->TopScoringPlayers;
				FString InfoTextString;
				if (TopPlayers.Num() == 0)
				{
					InfoTextString = FString(TEXT("There is no winner...\nOMEGALUL"));
				}
				else if (TopPlayers.Num() == 1)
				{
					if (TopPlayers[0] == BlasterPlayerState)
					{
						InfoTextString = FString(TEXT("YOU ARE THE WINNER!!"));
					}
					else
					{
						InfoTextString = FString::Printf(TEXT("Winner: \n%s\nBetter luck next time..."), *TopPlayers[0]->GetPlayerName());
					}
				}
				else if (TopPlayers.Num() > 1)
				{
					bool bWon = false;
					InfoTextString = FString("The match ended tied between:\n");
					for (auto TiedPlayer : TopPlayers)
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

void ABlasterPlayerController::SetStartingWeaponClass(TSubclassOf<AWeapon> WeaponClass)
{
	StartingWeaponClass = WeaponClass;
	if(!HasAuthority()) ServerSetStartingWeaponClass(WeaponClass);
}

void ABlasterPlayerController::ServerSetStartingWeaponClass_Implementation(TSubclassOf<AWeapon> WeaponClass)
{
	StartingWeaponClass = WeaponClass;
}
