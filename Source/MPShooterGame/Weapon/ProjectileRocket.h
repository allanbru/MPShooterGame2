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
	void DestroyTimerFinished();

	UPROPERTY(EditAnywhere)
	class UNiagaraSystem* TrailSystem{ nullptr };

	UPROPERTY()
	class UNiagaraComponent* TrailSystemComponent{ nullptr };

	UPROPERTY(EditAnywhere)
	USoundCue* ProjectileLoop{ nullptr };
	
	UPROPERTY()
	UAudioComponent* ProjectileLoopComponent{ nullptr };

	UPROPERTY(EditAnywhere);
	USoundAttenuation* LoopingSoundAttenuation{ nullptr };

	UPROPERTY(VisibleAnywhere)
	class URocketMovementComponent* RocketMovementComponent{ nullptr };

private:

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* RocketMesh;

	FTimerHandle DestroyTimer;

	UPROPERTY(EditAnywhere, Category = Explosion)
	float DestroyTime{ 3.f };

	UPROPERTY(EditAnywhere, Category=Explosion)
	float MinimumDamage{ 10.f };

	UPROPERTY(EditAnywhere, Category = Explosion)
	float InnerRadius{ 200.f };

	UPROPERTY(EditAnywhere, Category = Explosion)
	float OuterRadius{ 500.f };

	UPROPERTY(EditAnywhere, Category = Explosion)
	float DamageFalloff{ 1.f };
};
