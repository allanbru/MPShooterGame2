// Fill out your copyright notice in the Description page of Project Settings.


#include "FlagZone.h"
#include "Components/SphereComponent.h"
#include "MPShooterGame/GameMode/CaptureTheFlagGameMode.h"
#include "MPShooterGame/Weapon/Flag.h"

AFlagZone::AFlagZone()
{
	PrimaryActorTick.bCanEverTick = false;

	ZoneSphere = CreateDefaultSubobject<USphereComponent>(TEXT("ZoneSphere"));
	SetRootComponent(ZoneSphere);
}

void AFlagZone::BeginPlay()
{
	Super::BeginPlay();
	
	ZoneSphere->OnComponentBeginOverlap.AddDynamic(this, &AFlagZone::OnSphereOverlap);
}

void AFlagZone::OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{

	AFlag* OverlappingFlag = Cast<AFlag>(OtherActor);

	if (OverlappingFlag && OverlappingFlag->GetTeam() != Team)
	{
		ACaptureTheFlagGameMode* GameMode = Cast<ACaptureTheFlagGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			GameMode->FlagCaptured(OverlappingFlag, this);
		}
	}
}

