// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Pickup.h"
#include "HealthPickup.generated.h"

/**
 * 
 */
UCLASS()
class MPSHOOTERGAME_API AHealthPickup : public APickup
{
	GENERATED_BODY()

protected:
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult) override;

private:
	
	UPROPERTY(EditAnywhere)
	float HealAmount{ 100.f };

	UPROPERTY(EditAnywhere)
	float HealingTime{ 5.f };

	//Unused as for now, is the amount of remaining health in the pickup
	UPROPERTY(VisibleAnywhere)
	float HealthLeft{ 100.f };



};
