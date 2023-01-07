// Fill out your copyright notice in the Description page of Project Settings.


#include "HitScanWeapon.h"
#include "Engine/SkeletalMeshSocket.h"
#include "Kismet/GameplayStatics.h"
#include "MPShooterGame/BlasterComponents/LagCompensationComponent.h"
#include "MPShooterGame/Character/BlasterCharacter.h"
#include "MPShooterGame/PlayerController/BlasterPlayerController.h"
#include "Particles/ParticleSystemComponent.h"
#include "Sound/SoundCue.h"

void AHitScanWeapon::Fire(const FVector& HitTarget)
{
	Super::Fire(HitTarget);

	APawn* OwnerPawn = Cast<APawn>(GetOwner());
	if (OwnerPawn == nullptr) return;

	AController* InstigatorController = Cast<AController>(OwnerPawn->GetController());

	const USkeletalMeshSocket* MuzzleFlashSocket = GetWeaponMesh()->GetSocketByName(FName("MuzzleFlash"));
	if (MuzzleFlashSocket){

		FTransform SocketTransform = MuzzleFlashSocket->GetSocketTransform(GetWeaponMesh());
		FVector Start = SocketTransform.GetLocation();

		FHitResult FireHit;
		WeaponTraceHit(Start, HitTarget, FireHit);

		FCollisionQueryParams QueryParams = FCollisionQueryParams();
		QueryParams.AddIgnoredActor(OwnerPawn);

		UWorld* World = GetWorld();
		if (World)
		{

			ABlasterCharacter* BlasterCharacter = Cast<ABlasterCharacter>(FireHit.GetActor());
			if (BlasterCharacter && InstigatorController)
			{
				bool bCauseAuthoritativeDamage = !bUseServerSideRewind || OwnerPawn->IsLocallyControlled();
				if (bCauseAuthoritativeDamage && HasAuthority())
				{
					UGameplayStatics::ApplyDamage(
						BlasterCharacter,
						Damage,
						InstigatorController,
						this,
						UDamageType::StaticClass()
					);
				}
				else if (bUseServerSideRewind && !HasAuthority())
				{
					BlasterOwnerCharacter = (BlasterOwnerCharacter == nullptr) ? Cast<ABlasterCharacter>(OwnerPawn) : BlasterOwnerCharacter;
					BlasterOwnerController = (BlasterOwnerController == nullptr) ? Cast<ABlasterPlayerController>(InstigatorController) : BlasterOwnerController;
					if (BlasterOwnerCharacter && BlasterOwnerCharacter->IsLocallyControlled() && BlasterOwnerController && BlasterOwnerCharacter->GetLagCompensation())
					{
						BlasterOwnerCharacter->GetLagCompensation()->ServerScoreRequest(
							BlasterCharacter, 
							Start, 
							HitTarget, 
							BlasterOwnerController->GetServerTime() - BlasterOwnerController->SingleTripTime,
							this
						);
					}
				}
				

			}

			if (ImpactParticles)
			{
				UGameplayStatics::SpawnEmitterAtLocation(World, ImpactParticles, FireHit.ImpactPoint, FireHit.ImpactNormal.Rotation());
			}
			if (HitSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, HitSound, FireHit.ImpactPoint);
			}

			if (MuzzleFlash)
			{
				UGameplayStatics::SpawnEmitterAtLocation(World, MuzzleFlash, SocketTransform);
			}
			
			if (FireSound)
			{
				UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
			}
		}
		
	}
}

void AHitScanWeapon::WeaponTraceHit(const FVector& TraceStart, const FVector& HitTarget, FHitResult& OutHit)
{
	AActor* OwnerActor = GetOwner();
	if (OwnerActor == nullptr) return;

	UWorld* World = GetWorld();
	if (World)
	{
		FVector End = TraceStart + (HitTarget - TraceStart) * 1.01f;

		FCollisionQueryParams QueryParams = FCollisionQueryParams();
		QueryParams.AddIgnoredActor(OwnerActor);
		World->LineTraceSingleByChannel(
			OutHit,
			TraceStart,
			End,
			ECollisionChannel::ECC_Visibility,
			QueryParams
		);
		FVector BeamEnd = End;
		if (OutHit.bBlockingHit)
		{
			BeamEnd = OutHit.ImpactPoint;
		}
		if (BeamParticles)
		{
			UParticleSystemComponent* Beam = UGameplayStatics::SpawnEmitterAtLocation(World, BeamParticles, TraceStart, FRotator::ZeroRotator, true);
			if (Beam)
			{
				Beam->SetVectorParameter(FName("Target"), BeamEnd);
			}
		}
		
	}
}