// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "BlasterPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FHighPingDelegate, bool, bPingTooHigh);

/**
 * 
 */
UCLASS()
class MPSHOOTERGAME_API ABlasterPlayerController : public APlayerController
{
	GENERATED_BODY()

public:

	virtual void OnPossess(APawn* InPawn) override;
	virtual void Tick(float DeltaTime) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetHUDHealth(float Health, float MaxHealth);
	void SetHUDShield(float Shield, float MaxShield);
	void SetHUDScore(float Score);
	void SetHUDDefeats(int32 Defeats);
	void SetHUDWeaponAmmo(int32 Ammo);
	void SetHUDCarriedAmmo(int32 Ammo);
	void SetHUDGrenades(int32 Grenades);
	void SetHUDMatchCountdown(float CountdownTime);
	void SetHUDAnnouncementCountdown(float CountdownTime);

	UFUNCTION(BlueprintCallable)
	void SetHUDClassChange();

	virtual float GetServerTime(); //Synced with server world clock
	virtual void ReceivedPlayer() override; //Sync clocks ASAP
	
	void OnMatchStateSet(FName State);

	void HandleMatchHasStarted();
	void HandleCooldown();

	UFUNCTION(BlueprintCallable)
	void SetStartingWeaponClass(TSubclassOf<AWeapon> WeaponClass);

	float SingleTripTime{ 0.f };
	
	FHighPingDelegate HighPingDelegate;

protected:
	
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;
	
	void SetHUDTime();
	void PollInit();

	/*
	* Sync time between client and server
	*/

	UFUNCTION(Server, Reliable)
	void ServerRequestServerTime(float TimeOfClientRequest);
	
	UFUNCTION(Client, Reliable)
	void ClientReportServerTime(float TimeOfClientRequest, float TimeServerReceivedClientRequest);

	float ClientServerDelta{ 0.f }; //Difference between client and server time

	UPROPERTY(EditAnywhere, Category = Time)
	float TimeSyncFrequency{ 5.f };

	float TimeSyncRunningTime{ 0.f };
	void CheckTimeSync(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerCheckMatchState();

	UFUNCTION(Client, Reliable)
	void ClientJoinMidgame(FName StateOfMatch, float Warmup, float Match, float Cooldown, float StartingTime);

	void HighPingWarning();
	void StopHighPingWarning();
	void CheckPing(float DeltaTime);

private:

	UPROPERTY()
	class ABlasterHUD* BlasterHUD{ nullptr };

	UPROPERTY()
	class ABlasterGameMode* BlasterGameMode{ nullptr };
	
	float LevelStartingTime{ 0.f };
	float MatchTime{ 0.f };
	float WarmupTime{ 0.f };
	float CooldownTime{ 0.f };
	uint32 CountdownInt{ 0 };

	UPROPERTY(ReplicatedUsing = OnRep_MatchState)
	FName MatchState;
	
	UFUNCTION()
	void OnRep_MatchState();

	class UCharacterOverlay* CharacterOverlay{nullptr};

	float HUDHealth{ 0.f };
	float HUDMaxHealth{ 0.f };
	float HUDShield{ 0.f };
	float HUDMaxShield{ 0.f };
	float HUDScore{ 0.f };
	int32 HUDDefeats{ 0 };
	int32 HUDGrenades{ 0 };
	int32 HUDWeaponAmmo{ 0 };
	int32 HUDCarriedAmmo{ 0 };

	bool bInitializeHealth{ false };
	bool bInitializeShield{ false };
	bool bInitializeScore{ false };
	bool bInitializeDefeats{ false };
	bool bInitializeGrenades{ true };
	bool bInitializeWeaponAmmo{ false };
	bool bInitializeCarriedAmmo{ false };

	float HighPingRunningTime{ 0.f };
	
	UPROPERTY(EditAnywhere)
	float HighPingDuration{ 5.f };

	float PingAnimationRunningTime{ 0.f };

	UPROPERTY(EditAnywhere)
	float CheckPingFrequency{ 20.f };

	UFUNCTION(Server, Reliable)
	void ServerReportPingStatus(bool bHighPing);

	UPROPERTY(EditAnywhere)
	float HighPingThreshold{ 200.f };

	UPROPERTY(EditDefaultsOnly)
	TSubclassOf<AWeapon> StartingWeaponClass;

	UFUNCTION(Server, Reliable)
	void ServerSetStartingWeaponClass(TSubclassOf<AWeapon> WeaponClass);
};
