// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "MPShooterGame/BlasterTypes/CombatState.h"
#include "MPShooterGame/BlasterTypes/Team.h"
#include "MPShooterGame/BlasterTypes/TurningInPlace.h"
#include "MPShooterGame/Interfaces/InteractWithCrosshairsInterface.h"

#include "BlasterCharacter.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FOnLeftGame);

UCLASS()
class MPSHOOTERGAME_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:

	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	
	/**
	*	Play montages
	*/
	void PlayFireMontage(bool bAiming);
	void PlayReloadMontage();
	void PlayElimMontage();
	void PlayThrowGrenadeMontage();
	void PlaySwapMontage();

	virtual void OnRep_ReplicatedMovement() override;

	void Elim(bool bPlayerLeftGame); //Server Only
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim(bool bPlayerLeftGame);

	virtual void Destroyed() override;

	UPROPERTY()
	class ABlasterPlayerState* BlasterPlayerState{ nullptr };

	void SetStartingWeaponClass(TSubclassOf<AWeapon> WeaponClass);

	UFUNCTION(BlueprintCallable)
	void EquipStartingWeapon();

	UFUNCTION(Server, Reliable)
	void ServerEquipStartingWeapon();

	UPROPERTY(Replicated)
	bool bDisableGameplay{ false };

	UFUNCTION(BlueprintImplementableEvent)
	void ShowSniperScopeWidget(bool bShowScope);

	void UpdateHUDHealth();
	void UpdateHUDShield();
	void UpdateHUDAmmo();

	UPROPERTY()
	TMap<FName, class UBoxComponent*> HitCollisionBoxes;

	bool bFinishedSwapping{ true };

	FOnLeftGame OnLeftGame;

	UFUNCTION(Server, Reliable)
	void ServerLeaveGame();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastGainTheLead();

	UFUNCTION(NetMulticast, Reliable)
	void MulticastLostTheLead();

	void SetTeamColor(ETeam Team);

protected:
	
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);

	void EquipButtonPresssed();

	virtual void Jump() override;
	void CrouchButtonPressed();
	void CrouchButtonReleased();
	void ReloadButtonPressed();
	void AimButtonPressed();
	void AimButtonReleased();

	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	void SimProxiesTurn();
	void FireButtonPressed();
	void FireButtonReleased();
	void PlayHitReactMontage();
	void GrenadeButtonPressed();

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);

	//Poll for any relevant class and initialize our HUD
	void PollInit();
	void RotateInPlace(float DeltaTime);

	/**
	*	Hit boxes for server-side rewind
	*/
	UPROPERTY(EditAnywhere)
	UBoxComponent* head;

	UPROPERTY(EditAnywhere)
	UBoxComponent* pelvis;

	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_02;

	UPROPERTY(EditAnywhere)
	UBoxComponent* spine_03;

	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* upperarm_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* lowerarm_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* upperarm_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* hand_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* hand_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* backpack; 
	
	UPROPERTY(EditAnywhere)
	UBoxComponent* blanket;

	UPROPERTY(EditAnywhere)
	UBoxComponent* thigh_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* thigh_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* calf_r;

	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_l;

	UPROPERTY(EditAnywhere)
	UBoxComponent* foot_r;

private:

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom{ nullptr };

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget{ nullptr };

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon{ nullptr };

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	/**
	*	Blaster Components
	*/

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UCombatComponent* Combat{ nullptr };

	UPROPERTY(VisibleAnywhere)
	class UBuffComponent* Buff{ nullptr };

	UPROPERTY(VisibleAnywhere)
	class ULagCompensationComponent* LagCompensation;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	//UPROPERTY(Replicated)
	float AO_Yaw{ 0 };
	float Interp_AO_Yaw{ 0 };
	float AO_Pitch{ 0 };
	FRotator StartingAimRotation{ 0,0,0 };

	ETurningInPlace TurningInPlace{ETurningInPlace::ETIP_NotTurning};
	void TurnInPlace(float DeltaTime);

	/*
	* Animation Montages
	*/

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* FireWeaponMontage{ nullptr };

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* ReloadMontage{ nullptr };

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage{ nullptr };

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ElimMontage { nullptr };

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ThrowGranadeMontage { nullptr };

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* SwapMontage { nullptr };

	void HideCameraIfCharacterClose();

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	bool bRotateRootBone{ false };
	float TurnThreshold{ 5.f };
	FRotator ProxyRotationLastFrame{0,0,0};
	FRotator ProxyRotation{ 0,0,0 };
	float ProxyYaw{ 0.f };
	float TimeSinceLastMovementReplication{ 0.f };
	float CalculateSpeed();

	/**
	* PlayerHealth
	*/

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth{ 100.f };

	UPROPERTY(ReplicatedUsing=OnRep_Health, VisibleAnywhere, Category="Player Stats")
	float Health{ 100.f };

	UFUNCTION()
	void OnRep_Health(float LastHealth);


	/**
	* Player Shield
	*/

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxShield{ 100.f };

	UPROPERTY(ReplicatedUsing=OnRep_Shield, VisibleAnywhere, Category="Player Stats")
	float Shield{ 0.f };

	UFUNCTION()
	void OnRep_Shield(float LastShield);

	//General

	class ABlasterPlayerController* BlasterPlayerController{ nullptr };

	bool bElimmed{ false };

	FTimerHandle ElimTimer;

	UPROPERTY(EditDefaultsOnly)
	float ElimDelay{ 3.f };

	void ElimTimerFinished();

	bool bLeftGame{ false };
	/**
	* Dissolve Effect
	*/

	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline{ nullptr };
	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve{ nullptr };

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void StartDissolve();
	
	//Dyn Inst changed at runtime
	UPROPERTY(VisibleAnywhere, Category = Elim)
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance{ nullptr };

	//Mat Inst set on BP, used w/ the DynMatInst
	UPROPERTY(VisibleAnywhere, Category = Elim)
	UMaterialInstance* DissolveMaterialInstance{ nullptr };

	/**
	* Team Colors
	*/

	UPROPERTY(EditAnywhere, Category = Team)
	UMaterialInstance* RedDissolveMatInst;

	UPROPERTY(EditAnywhere, Category = Team)
	UMaterialInstance* RedMaterial;
	
	UPROPERTY(EditAnywhere, Category = Team)
	UMaterialInstance* BlueDissolveMatInst;

	UPROPERTY(EditAnywhere, Category = Team)
	UMaterialInstance* BlueMaterial;

	UPROPERTY(EditAnywhere, Category = Team)
		UMaterialInstance* OriginalMaterial;

	/**
	* Elim effects
	*/

	UPROPERTY(EditAnywhere)
	UParticleSystem* ElimBotEffect{ nullptr };

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ElimBotComponent{ nullptr };

	UPROPERTY(EditAnywhere)
	class USoundCue* ElimBotSound{ nullptr };

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* CrownSystem;

	UPROPERTY()
	class UNiagaraComponent* CrownComponent;
	
	/**
	* Grenade
	*/
	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* AttachedGrenade;

	/**
	* Starting Weapon
	*/
	UPROPERTY(EditAnywhere, Category=Combat)
	TSubclassOf<AWeapon>StartingWeaponClass{ nullptr };

	UPROPERTY(EditAnywhere, Category = Combat)
	float InvulnerabilityTime{ 3.f };

	void SetVulnerable();
	FTimerHandle InvulnerabilityTimer;

	class ABlasterGameMode* BlasterGameMode;

public:	
	
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	FORCEINLINE float GetAO_Yaw() { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() { return AO_Pitch; }
	AWeapon* GetEquippedWeapon();
	FORCEINLINE ETurningInPlace GetTurningInPlace() { return TurningInPlace; }
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE void SetHealth(float Amount) { Health = Amount; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }
	FORCEINLINE float GetShield() const { return Shield; }
	FORCEINLINE void SetShield(float Amount) { Shield = Amount; }
	FORCEINLINE float GetMaxShield() const { return MaxShield; }
	ECombatState GetCombatState() const;
	FORCEINLINE UCombatComponent* GetCombat() const { return Combat; }
	FORCEINLINE bool GetDisableGameplay() const { return bDisableGameplay; }
	FORCEINLINE UAnimMontage* GetReloadMontage() const { return ReloadMontage; }
	FORCEINLINE UStaticMeshComponent* GetAttachedGrenade() const { return AttachedGrenade; }
	FORCEINLINE UBuffComponent* GetBuff() const { return Buff; }
	bool IsLocallyReloading();
	FORCEINLINE ULagCompensationComponent* GetLagCompensation() { return LagCompensation; }
	bool IsHoldingTheFlag() const;

};
