// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "MPShooterGame/Weapon/WeaponTypes.h"
#include "AmmoPickup.generated.h"

/**
 * 
 */
UCLASS()
class MPSHOOTERGAME_API AAmmoPickup : public APickup
{
	GENERATED_BODY()

public:

	virtual void BeginPlay() override;

protected:

	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:

	//Used only if random ammo is true
	UPROPERTY(EditAnywhere, Category = "Settings")
	int32 MinAmmoAmount{ 15 };

	UPROPERTY(EditAnywhere, Category = "Settings")
	int32 AmmoAmount{ 30 };

	//If random ammo is true, the pickup amount will be a random value from MinAmmoAmount to AmmoAmount
	UPROPERTY(EditAnywhere, Category = "Settings")
	bool bRandomAmmo{ false };

	UPROPERTY(EditAnywhere, Category = "Settings")
	EWeaponType WeaponType;
	
};
