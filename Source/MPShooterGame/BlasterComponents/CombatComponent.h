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
	void Reload();
	UFUNCTION(BlueprintCallable)
	void FinishReloading();

	void FireButtonPressed(bool bPressed);

	float TraceLength{ 80000.f };

protected:

	virtual void BeginPlay() override;

	void SetAiming(bool bIsAiming);

	UFUNCTION(Server, Reliable)
	void ServerSetAiming(bool bIsAiming);

	UFUNCTION()
	void OnRep_EquippedWeapon();

	void Fire();

	UFUNCTION(Server, Reliable)
	void ServerFire(const FVector_NetQuantize& TraceHitTarget);

	UFUNCTION(NetMulticast, Reliable)
	void MulticastFire(const FVector_NetQuantize& TraceHitTarget);

	void TraceUnderCrosshairs(FHitResult& HitResult, FVector& End);

	void SetHUDCrosshairs(float DeltaTime);

	UFUNCTION(Server, Reliable)
	void ServerReload();

	int32 AmountToReload();

	void UpdateAmmoValues();

	void HandleReload();

private:

	ABlasterCharacter* Character{ nullptr };
	ABlasterPlayerController* Controller{ nullptr };
	ABlasterHUD* HUD{ nullptr };
	
	UPROPERTY(ReplicatedUsing = OnRep_EquippedWeapon)
	AWeapon* EquippedWeapon{ nullptr };

	UPROPERTY(Replicated)
	bool bAiming{ false };

	UPROPERTY(EditAnywhere)
	float BaseWalkSpeed{ 600.f };

	UPROPERTY(EditAnywhere)
	float AimWalkSpeed{ 450.f };

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
	int32 StartingARAmmo{ 30 };

	UPROPERTY(EditAnywhere)
	int32 StartingRocketAmmo{ 4 };

	UPROPERTY(EditAnywhere)
	int32 StartingPistolAmmo{ 30 };

	UPROPERTY(EditAnywhere)
	int32 StartingSMGAmmo { 120 };

	UPROPERTY(EditAnywhere)
	int32 StartingShotgunAmmo { 15 };

	UPROPERTY(EditAnywhere)
	int32 StartingSniperAmmo{ 15 };

	void InitializeCarriedAmmo();

	UPROPERTY(ReplicatedUsing = OnRep_CombatState)
	ECombatState CombatState{ ECombatState::ECS_Unoccupied };

	UFUNCTION()
	void OnRep_CombatState();

public:	

};
