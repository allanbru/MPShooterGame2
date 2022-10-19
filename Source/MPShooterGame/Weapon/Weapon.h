// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Weapon.generated.h"

UENUM(BlueprintType)
enum class EWeaponState : uint8 {
	EWS_Initial UMETA(DisplayName = "IntialState"),
	EWS_Equipped UMETA(DisplayName = "Equipped"),
	EWS_Dropped UMETA(DisplayName = "Dropped"),
	EWS_Max UMETA(DisplayName = "DefaultMAX"),
};

UCLASS()
class MPSHOOTERGAME_API AWeapon : public AActor
{
	GENERATED_BODY()
	
public:	

	AWeapon();
	virtual void Tick(float DeltaTime) override;
	void ShowPickupWidget(bool bShowWidget);
	virtual void Fire(const FVector& HitTarget);

protected:

	virtual void BeginPlay() override;

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	UFUNCTION()
	virtual void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	virtual void OnSphereEndOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);

private:
	
	UPROPERTY(VisibleAnywhere, Category = "WeaponProperties")
	USkeletalMeshComponent* WeaponMesh{ nullptr };

	UPROPERTY(VisibleAnywhere, Category = "WeaponProperties")
	class USphereComponent* AreaSphere{ nullptr };

	UPROPERTY(ReplicatedUsing = OnRep_WeaponState, VisibleAnywhere, Category = "WeaponProperties")
	EWeaponState WeaponState{ EWeaponState::EWS_Initial };

	UFUNCTION()
	void OnRep_WeaponState();

	UPROPERTY(VisibleAnywhere, Category = "WeaponProperties")
	class UWidgetComponent* PickupWidget{ nullptr };

	UPROPERTY(EditAnywhere, Category = "WeaponProperties")
	class UAnimationAsset* FireAnimation;

	UPROPERTY(EditAnywhere)
	TSubclassOf<class ACasing>CasingClass{ nullptr };
	
public:
	void SetWeaponState(EWeaponState State);
	FORCEINLINE USphereComponent* GetAreaSphere() { return AreaSphere; };
	FORCEINLINE USkeletalMeshComponent* GetWeaponMesh() const { return WeaponMesh; };
};
