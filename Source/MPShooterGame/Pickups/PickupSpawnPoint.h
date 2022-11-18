// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "PickupSpawnPoint.generated.h"

UCLASS()
class MPSHOOTERGAME_API APickupSpawnPoint : public AActor
{
	GENERATED_BODY()
	
public:	
	APickupSpawnPoint();
	virtual void Tick(float DeltaTime) override;
protected:
	virtual void BeginPlay() override;

	UPROPERTY(EditAnywhere)
	TArray<TSubclassOf<class APickup>> PickupClasses;

	UPROPERTY()
	APickup* SpawnedPickup{ nullptr };

	void SpawnPickup();

	UFUNCTION()
	void StartSpawnPickupTimer(AActor* DestroyedActor);
	
	void SpawnPickupTimerFinished();
	

private:

	FTimerHandle SpawnPickupTimer;

	UPROPERTY(EditAnywhere)
	float SpawnPickupTimeMin{ 5.f };
	UPROPERTY(EditAnywhere)
	float SpawnPickupTimeMax{ 5.f };

public:	


};
