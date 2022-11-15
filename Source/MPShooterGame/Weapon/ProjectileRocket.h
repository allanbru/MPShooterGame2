// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Projectile.h"
#include "ProjectileRocket.generated.h"

/**
 * 
 */
UCLASS()
class MPSHOOTERGAME_API AProjectileRocket : public AProjectile
{
	GENERATED_BODY()

public:

	AProjectileRocket();

	virtual void Destroyed() override;

protected:

	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit) override;
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	USoundCue* ProjectileLoop{ nullptr };
	
	UPROPERTY()
	UAudioComponent* ProjectileLoopComponent{ nullptr };

	UPROPERTY(EditAnywhere);
	USoundAttenuation* LoopingSoundAttenuation{ nullptr };

	UPROPERTY(VisibleAnywhere)
	class URocketMovementComponent* RocketMovementComponent{ nullptr };

private:

};
