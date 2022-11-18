// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "ShieldPickup.generated.h"

/**
 * 
 */
UCLASS()
class MPSHOOTERGAME_API AShieldPickup : public APickup
{
	GENERATED_BODY()

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:

	UPROPERTY(EditAnywhere)
	float ShieldReplenishAmount{ 100.f };

	UPROPERTY(EditAnywhere)
	float ShieldReplenishTime{ 5.f };

	//Unused as for now, is the amount of remaining health in the pickup
	UPROPERTY(VisibleAnywhere)
	float ShieldReplenishLeft{ 100.f };
	
};
