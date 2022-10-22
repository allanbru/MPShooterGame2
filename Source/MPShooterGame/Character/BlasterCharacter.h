// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/TimelineComponent.h"
#include "GameFramework/Character.h"
#include "MPShooterGame/BlasterTypes/TurningInPlace.h"
#include "MPShooterGame/Interfaces/InteractWithCrosshairsInterface.h"

#include "BlasterCharacter.generated.h"


UCLASS()
class MPSHOOTERGAME_API ABlasterCharacter : public ACharacter, public IInteractWithCrosshairsInterface
{
	GENERATED_BODY()

public:

	ABlasterCharacter();
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	virtual void PostInitializeComponents() override;
	void PlayFireMontage(bool bAiming);
	void PlayElimMontage();
	virtual void OnRep_ReplicatedMovement() override;

	void Elim(); //Server Only
	UFUNCTION(NetMulticast, Reliable)
	void MulticastElim();

	virtual void Destroyed() override;

	UPROPERTY()
	class ABlasterPlayerState* BlasterPlayerState{ nullptr };

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

	void AimOffset(float DeltaTime);
	void CalculateAO_Pitch();
	void SimProxiesTurn();
	void FireButtonPressed();
	void FireButtonReleased();
	void PlayHitReactMontage();

	UFUNCTION()
	void ReceiveDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, class AController* InstigatorController, AActor* DamageCauser);
	void UpdateHUDHealth();

	//Poll for any relevant class and initialize our HUD
	void PollInit();

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
	class UCombatComponent* Combat{ nullptr };

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

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* HitReactMontage{ nullptr };

	UPROPERTY(EditAnywhere, Category = Combat)
	UAnimMontage* ElimMontage { nullptr };

	void HideCameraIfCharacterClose();

	UPROPERTY(EditAnywhere)
	float CameraThreshold = 200.f;

	bool bRotateRootBone{ false };
	float TurnThreshold{ 5.f };
	FRotator ProxyRotationLastFrame{0,0,0};
	FRotator ProxyRotation{ 0,0,0 };
	float ProxyYaw{ 0.f };
	float TimeSinceLastMovementReplication{ 0.f };
	float CalculateSpeed();

	/**
	* PlayerHealth
	*/

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	float MaxHealth{ 100.f };

	UPROPERTY(ReplicatedUsing=OnRep_Health, VisibleAnywhere, Category="Player Stats")
	float Health{ 100.f };

	UFUNCTION()
	void OnRep_Health();

	class ABlasterPlayerController* BlasterPlayerController{ nullptr };

	bool bElimmed{ false };

	FTimerHandle ElimTimer;

	UPROPERTY(EditDefaultsOnly)
	float ElimDelay{ 3.f };

	void ElimTimerFinished();

	/**
	* Dissolve Effect
	*/

	UPROPERTY(VisibleAnywhere)
	UTimelineComponent* DissolveTimeline{ nullptr };
	FOnTimelineFloat DissolveTrack;

	UPROPERTY(EditAnywhere)
	UCurveFloat* DissolveCurve{ nullptr };

	UFUNCTION()
	void UpdateDissolveMaterial(float DissolveValue);
	void StartDissolve();
	
	//Dyn Inst changed at runtime
	UPROPERTY(VisibleAnywhere, Category = Elim)
	UMaterialInstanceDynamic* DynamicDissolveMaterialInstance{ nullptr };

	//Mat Inst set on BP, used w/ the DynMatInst
	UPROPERTY(EditAnywhere, Category = Elim)
	UMaterialInstance* DissolveMaterialInstance{ nullptr };

	/**
	* Elim bot effects
	*/

	UPROPERTY(EditAnywhere)
	UParticleSystem* ElimBotEffect{ nullptr };

	UPROPERTY(VisibleAnywhere)
	UParticleSystemComponent* ElimBotComponent{ nullptr };

	UPROPERTY(EditAnywhere)
	class USoundCue* ElimBotSound{ nullptr };

public:	
	
	void SetOverlappingWeapon(AWeapon* Weapon);
	bool IsWeaponEquipped();
	bool IsAiming();
	FORCEINLINE float GetAO_Yaw() { return AO_Yaw; }
	FORCEINLINE float GetAO_Pitch() { return AO_Pitch; }
	AWeapon* GetEquippedWeapon();
	FORCEINLINE ETurningInPlace GetTurningInPlace() { return TurningInPlace; }
	FVector GetHitTarget() const;
	FORCEINLINE UCameraComponent* GetFollowCamera() const { return FollowCamera; }
	FORCEINLINE bool ShouldRotateRootBone() const { return bRotateRootBone; }
	FORCEINLINE bool IsElimmed() const { return bElimmed; }
	FORCEINLINE float GetHealth() const { return Health; }
	FORCEINLINE float GetMaxHealth() const { return MaxHealth; }

};
