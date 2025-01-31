// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MPShooterGame/BlasterTypes/CombatState.h"
#include "MPShooterGame/HUD/BlasterHUD.h"
#include "MPShooterGame/Weapon/WeaponTypes.h"
#include "CombatComponent.generated.h"

class ABlasterHUD;
class ABlasterPlayerController;
class AWeapon;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MPSHOOTERGAME_API UCombatComponent : public UActorComponent
{
	GENERATED_BODY()

public:	

	UCombatComponent();
	friend class ABlasterCharacter;	
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void EquipWeapon(AWeapon* WeaponToEquip);
	void SwapWeapons();
	void Reload();
	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	UFUNCTION(BlueprintCallable)
	void FinishSwap();

	UFUNCTION(BlueprintCallable)
	void FinishSwapAttachedWeapons();

	void FireButtonPressed(bool bPressed);

	UFUNCTION(BlueprintCallable)
	void ShotgunShellReload();
	void JumpToShotgunEnd();

	UFUNCTION(BlueprintCallable)
	void ThrowGrenadeFinished();

	UFUNCTION(BlueprintCallable)
	void LaunchGrenade();

	UFUNCTION(Server, Reliable)
	void ServerLaunchGrenade(const FVector_NetQuantize& Target);

	void PickupAmmo(EWeaponType WeaponType, int32 AmmoAmount);

	bool bLocallyReloading{ false };

protected:

	virtual void BeginPlay() override;

	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	UFUNCTION()
	void OnRep_SecondaryWeapon();

	void Fire();
	void FireProjectileWeapon();
	void FireHitScanWeapon();
	void FireShotgun();
	void LocalFire(const FVector_NetQuantize& TraceHitTarget);
	void ShotgunLocalFire(const TArray<FVector_NetQuantize>& TraceHitTargets);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget, float FireDelay);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(Server, Reliable, WithValidation)
	void ServerShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets, float FireDelay);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastShotgunFire(const TArray<FVector_NetQuantize>& TraceHitTargets);

	void TraceUnderCrosshairs(FHitResult& HitResult, FVector& End);

	void SetHUDCrosshairs(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerReload();

	void HandleReload();
	int32 AmountToReload();

	void ThrowGrenade();

	UFUNCTION(Server, Reliable)
	void ServerThrowGrenade();

	UPROPERTY(EditAnywhere)
	TSubclassOf<class AProjectile> GrenadeClass;

	void UpdateAmmoValues();

	void UpdateShotgunAmmoValues();

	void DropEquippedWeapon();
	void AttachActorToRightHand(AActor* ActorToAttach);
	void AttachActorToLeftHand(AActor* ActorToAttach);
	void AttachFlagToLeftHand(AWeapon* Flag);
	void AttachActorToBackpack(AActor* ActorToAttach);
	void UpdateCarriedAmmo();
	void PlayEquipWeaponSound(AWeapon* WeaponToEquip);
	void ReloadEmptyWeapon();

	void ShowAttachedGrenade(bool bShowGrenade);

	void EquipPrimaryWeapon(AWeapon* WeaponToEquip);
	void EquipSecondaryWeapon(AWeapon* WeaponToEquip);

private:

	ABlasterCharacter* Character{ nullptr };
	ABlasterPlayerController* Controller{ nullptr };
	ABlasterHUD* HUD{ nullptr };
	
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon{ nullptr };

	UPROPERTY(ReplicatedUsing = OnRep_SecondaryWeapon)
	AWeapon* SecondaryWeapon{ nullptr };

	UPROPERTY(ReplicatedUsing = OnRep_Aiming)
	bool bAiming{ false };

	bool bAimButtonPressed{ false };
	
	UFUNCTION()
	void OnRep_Aiming();

	UPROPERTY(EditAnywhere, Category = "Movement")
	float BaseWalkSpeed{ 600.f };

	UPROPERTY(EditAnywhere, Category = "Movement")
	float AimWalkSpeed{ 450.f };

	UPROPERTY(EditAnywhere, Category = "Movement")
	float RocketLaunchSpeed{ 1000.f };

	bool bFireButtonPressed{ false };

	/**
	* HUD and Crosshairs
	*/

	float CrosshairVelocityFactor{ 0.f };
	float CrosshairInAirFactor{ 0.f };
	float CrosshairAimFactor{ 0.f };
	float CrosshairShootingFactor{ 0.f };

	FVector HitTarget{ 0,0,0 };

	FHUDPackage HUDPackage;

	/**
	* Aiming and FOV
	*/

	//FOV when not aiming, set to camera's base FOV in BeginPlay
	float DefaultFOV{ 45.f };

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomedFOV{ 30.f };

	float CurrentFOV{ 45.f };

	UPROPERTY(EditAnywhere, Category = Combat)
	float ZoomInterpSpeed{ 20.f };

	void InterpFOV(float DeltaTime);

	/**
	* Automatic fire
	*/

	FTimerHandle FireTimer;

	bool bCanFire{ true };

	void StartFireTimer();
	void FireTimerFinished();

	bool CanFire();

	//CarriedAmmo for the currently EquippedWeapon type
	UPROPERTY(ReplicatedUsing = OnRep_CarriedAmmo)
	int32 CarriedAmmo{ 0 };

	UFUNCTION()
	void OnRep_CarriedAmmo();

	TMap<EWeaponType, int32> CarriedAmmoMap;

	UPROPERTY(EditAnywhere)
	int32 MaxCarriedAmmo{ 500 };

	UPROPERTY(EditAnywhere)
	int32 StartingARAmmo{ 30 };

	UPROPERTY(EditAnywhere)
	int32 StartingRocketAmmo{ 4 };

	UPROPERTY(EditAnywhere)
	int32 StartingPistolAmmo{ 30 };

	UPROPERTY(EditAnywhere)
	int32 StartingSMGAmmo { 120 };

	UPROPERTY(EditAnywhere)
	int32 StartingShotgunAmmo { 21 };

	UPROPERTY(EditAnywhere)
	int32 StartingSniperAmmo{ 15 };

	UPROPERTY(EditAnywhere)
	int32 StartingGrenadeLauncherAmmo{ 8 };

	void InitializeCarriedAmmo();

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState{ ECombatState::ECS_Unoccupied };

	UFUNCTION()
	void OnRep_CombatState();

	UPROPERTY(EditAnywhere, ReplicatedUsing = OnRep_Grenades)
	int32 Grenades{ 3 };

	UFUNCTION()
	void OnRep_Grenades();

	UPROPERTY(EditAnywhere)
	int32 MaxGrenades{ 3 };

	void UpdateHUDGrenades();

	UPROPERTY(ReplicatedUsing = OnRep_HoldingTheFlag)
	bool bHoldingTheFlag{ false };

	UFUNCTION()
	void OnRep_HoldingTheFlag();

	UPROPERTY()
	AWeapon* TheFlag;

	UPROPERTY(EditDefaultsOnly)
	bool bBurdenFlagCarrier{ false };

public:	

	float TraceLength{ 80000.f };

	FORCEINLINE int32 GetGrenades() const { return Grenades; }
	bool ShouldSwapWeapons();

};
