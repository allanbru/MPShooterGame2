// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "HitScanWeapon.generated.h"

/**
 * 
 */
UCLASS()
class MPSHOOTERGAME_API AHitScanWeapon : public AWeapon
{
	GENERATED_BODY()

public:

	virtual void Fire(const FVector& HitTarget) override;

protected:

	void WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit);
	
	UPROPERTY(EditAnywhere)
	float Damage{ 20.f };

	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles{ nullptr };

	UPROPERTY(EditAnywhere)
	UParticleSystem* BeamParticles{ nullptr };

	UPROPERTY(EditAnywhere)
	UParticleSystem* MuzzleFlash{ nullptr };

	UPROPERTY(EditAnywhere)
	USoundCue* FireSound{ nullptr };

	UPROPERTY(EditAnywhere)
	USoundCue* HitSound { nullptr };

};
