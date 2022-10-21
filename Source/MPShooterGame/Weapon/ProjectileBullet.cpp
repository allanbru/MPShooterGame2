// Fill out your copyright notice in the Description page of Project Settings.


#include "ProjectileBullet.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

void AProjectileBullet::OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit)
{
	ACharacter* OwnerCharacter = Cast<ACharacter>(GetOwner());
	if (!OwnerCharacter) return;
	AController* OwnerController = OwnerCharacter->Controller;
	if (!OwnerController) return;
	if(OtherActor->GetOwner())
	UE_LOG(LogTemp, Warning, TEXT("Hit something!! Damage = %.1f"), Damage);
	UGameplayStatics::ApplyDamage(OtherActor, Damage, OwnerController, this, UDamageType::StaticClass());
	UE_LOG(LogTemp, Warning, TEXT("Damage should've been applied!!"));
	Super::OnHit(HitComp, OtherActor, OtherComp, NormalImpulse, Hit);
}
