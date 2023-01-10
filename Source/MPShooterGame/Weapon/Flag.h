// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Weapon.h"
#include "Flag.generated.h"

/**
 * 
 */
UCLASS()
class MPSHOOTERGAME_API AFlag : public AWeapon
{
	GENERATED_BODY()
	
public:

	AFlag();

	virtual void Dropped() override;
	void ResetFlag();

protected:

	virtual void BeginPlay() override;

	virtual void OnEquipped() override;
	virtual void OnDropped() override;

	virtual void Destroyed() override;

private:
	UPROPERTY(VisibleAnywhere)
	class USkeletalMeshComponent* FlagMesh;

	FVector InitialLocation{ 0.f,0.f,0.f };
	FRotator InitialRotation{ 0.f,0.f,0.f };

	bool bInBase{ true };

public:

	FORCEINLINE bool IsInBase() const { return bInBase; }
};
