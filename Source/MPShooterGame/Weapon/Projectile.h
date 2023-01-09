// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "Projectile.generated.h"

UCLASS()
class MPSHOOTERGAME_API AProjectile : public AActor
{
	GENERATED_BODY()
	
public:	
	AProjectile();
	virtual void Tick(float DeltaTime) override;
	virtual void Destroyed() override;


	/**
	* Used with Server-Side Rewind
	*/

	UPROPERTY(EditAnywhere)
	bool bUseServerSideRewind{ false };

	FVector_NetQuantize TraceStart{ 0.f, 0.f, 0.f };
	FVector_NetQuantize100 InitialVelocity{ 0.f, 0.f, 0.f };

	UPROPERTY(EditAnywhere)
	float InitialSpeed{ 15000.f };

	// Only has effect in Grenades and Rockets, since projectile weapons damage overrides this damage upon spawning.
	UPROPERTY(EditAnywhere)
	float Damage{ 20.f };

	// No effects at all. Projectiles weapon override this value upon spawning.
	UPROPERTY(EditAnywhere)
	float HeadshotDamage{ 40.f };

	UPROPERTY()
	class AWeapon* OwningWeapon;

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);
	void StartDestroyTimer();
	void DestroyTimerFinished();
	void SpawnTrailSystem();
	void ExplodeDamage();

	UPROPERTY(EditAnywhere)
	class UParticleSystem* ImpactParticles;

	UPROPERTY(EditAnywhere)
	class USoundCue* ImpactSound;

	UPROPERTY(EditAnywhere)
	class UBoxComponent* CollisionBox;

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* TrailSystem{ nullptr };

	UPROPERTY()
	class UNiagaraComponent* TrailSystemComponent{ nullptr };

	UPROPERTY(VisibleAnywhere)
	class UProjectileMovementComponent* ProjectileMovementComponent;

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* ProjectileMesh;

	UPROPERTY(EditAnywhere, Category = Explosion)
	float MinimumDamage{ 10.f };

	UPROPERTY(EditAnywhere, Category = Explosion)
	float InnerRadius{ 200.f };

	UPROPERTY(EditAnywhere, Category = Explosion)
	float OuterRadius{ 500.f };

	UPROPERTY(EditAnywhere, Category = Explosion)
	float DamageFalloff{ 1.f };

private:
	
	UPROPERTY(EditAnywhere)
	UParticleSystem* Tracer;

	class UParticleSystemComponent* TracerComponent;	

	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere, Category = Explosion)
	float DestroyTime{ 3.f };

public:	
	

};
