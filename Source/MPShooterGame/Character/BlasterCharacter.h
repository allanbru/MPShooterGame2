// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "MPShooterGame/BlasterTypes/TurningInPlace.h"

#include "BlasterCharacter.generated.h"


UCLASS()
class MPSHOOTERGAME_API ABlasterCharacter : public ACharacter
{
	GENERATED_BODY()

public:

	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void PlayFireMontage(bool bAiming);

protected:
	
	virtual void BeginPlay() override;

	void MoveForward(float Value);
	void MoveRight(float Value);
	void Turn(float Value);
	void LookUp(float Value);

	void EquipButtonPresssed();
	void CrouchButtonPressed();
	void CrouchButtonReleased();
	void AimButtonPressed();
	void AimButtonReleased();
	void FireButtonPressed();
	void FireButtonReleased();

	void AimOffset(float DeltaTime);

private:

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class USpringArmComponent* CameraBoom{ nullptr };

	UPROPERTY(VisibleAnywhere, Category = Camera)
	class UCameraComponent* FollowCamera{ nullptr };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	class UWidgetComponent* OverheadWidget{ nullptr };

	UPROPERTY(ReplicatedUsing = OnRep_OverlappingWeapon)
	class AWeapon* OverlappingWeapon{ nullptr };

	UFUNCTION()
	void OnRep_OverlappingWeapon(AWeapon* LastWeapon);

	UPROPERTY(VisibleAnywhere)
	class UCombatComponent* Combat;

	UFUNCTION(Server, Reliable)
	void ServerEquipButtonPressed();

	//UPROPERTY(Replicated)
	float AO_Yaw{ 0 };
	float Interp_AO_Yaw{ 0 };
	float AO_Pitch{ 0 };
	FRotator StartingAimRotation{ 0,0,0 };

	ETurningInPlace TurningInPlace;
	void TurnInPlace(float DeltaTime);

	UPROPERTY(EditAnywhere, Category = Combat)
	class UAnimMontage* FireWeaponMontage{ nullptr };

public:	
	
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	FORCEINLINE float GetAO_Yaw() { return AO_Yaw; };
	FORCEINLINE float GetAO_Pitch() { return AO_Pitch; };
	AWeapon* GetEquippedWeapon();
	FORCEINLINE ETurningInPlace GetTurningInPlace() { return TurningInPlace; }

};
