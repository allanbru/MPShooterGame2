// Fill out your copyright notice in the Description page of Project Settings.


#include "Shotgun.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "Kismet/KismetMathLibrary.h"
#include "MPShooterGame/BlasterComponents/LagCompensationComponent.h"
#include "MPShooterGame/Character/BlasterCharacter.h"
#include "MPShooterGame/PlayerController/BlasterPlayerController.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

void AShotgun::FireShotgun(const TArray<FVector_NetQuantize>& HitTargets)
{
	AWeapon::Fire(FVector());
	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;

	AController* InstigatorController = Cast<AController>(OwnerPawn->GetController());
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket)
	{
		const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		const FVector Start = SocketTransform.GetLocation();

		//Maps each hit character to number of times hit
		TMap<ABlasterCharacter*, uint32> HitMap;
		for (FVector_NetQuantize HitTarget : HitTargets)
		{
			FHitResult FireHit;
			WeaponTraceHit(Start, HitTarget, FireHit);

			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
			if (BlasterCharacter)
			{
				if (HitMap.Contains(BlasterCharacter))
				{
					HitMap[BlasterCharacter]++;
				}
				else {
					HitMap.Emplace(BlasterCharacter, 1);
				}

			}

			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ImpactParticles, FireHit.ImpactPoint, FireHit.ImpactNormal.Rotation());
			}

			if (HitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, HitSound, FireHit.ImpactPoint, .5f, FMath::FRandRange(-.5f, .5f));
			}
		}
		
		//Apply damage needs instigator controller
		if (InstigatorController)
		{
			TArray<ABlasterCharacter*> HitCharacters;
			for (auto HitPair : HitMap)
			{
				if (HitPair.Key)
				{
					HitCharacters.Add(HitPair.Key);
					bool bCauseAuthoritativeDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
					if (bCauseAuthoritativeDamage && HasAuthority())
					{
						UGameplayStatics::ApplyDamage(
							HitPair.Key, // Character hit
							Damage * HitPair.Value, // Damage * #hits
							InstigatorController,
							this,
							UDamageType::StaticClass()
						);
					}
				}
			}
			if (bUseServerSideRewind && !HasAuthority())
			{
				BlasterOwnerCharacter = (BlasterOwnerCharacter == nullptr) ? Cast<ABlasterCharacter>(OwnerPawn) : BlasterOwnerCharacter;
				BlasterOwnerController = (BlasterOwnerController == nullptr) ? Cast<ABlasterPlayerController>(InstigatorController) : BlasterOwnerController;
				if (BlasterOwnerCharacter && BlasterOwnerCharacter->IsLocallyControlled() && BlasterOwnerController && BlasterOwnerCharacter->GetLagCompensation())
				{
					BlasterOwnerCharacter->GetLagCompensation()->ShotgunServerScoreRequest(
						HitCharacters, 
						Start, 
						HitTargets, 
						BlasterOwnerController->GetServerTime() - BlasterOwnerController->SingleTripTime, 
						this
					);
				}
			}
		}
	}
}

void AShotgun::ShotgunTraceEndWithScatter(const FVector& HitTarget, TArray<FVector_NetQuantize>& HitTargets)
{
	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket == nullptr) return;

	const FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
	const FVector TraceStart = SocketTransform.GetLocation();
	const FVector ToTargetNormalize = (HitTarget - TraceStart).GetSafeNormal();
	const FVector SphereCenter = TraceStart + ToTargetNormalize * DistanceToSphere;

	for (uint32 i = 0; i < NumberOfPellets; i++)
	{
		const FVector RandVec = UKismetMathLibrary::RandomUnitVector() * FMath::FRandRange(0.f, SphereRadius);
		const FVector EndLoc = SphereCenter + RandVec;
		const FVector ToEndLoc = EndLoc - TraceStart;

		HitTargets.Add(FVector(TraceStart + ToEndLoc.GetSafeNormal() * TraceLength));
	}
}
