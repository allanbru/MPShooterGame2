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
	UE_LOG(LogTemp, Warning, TEXT("Overlapping"));
	if (OtherActor && OtherActor->GetName().Len() > 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("Actor: %s"), *OtherActor->GetName());
	}
	AFlag* OverlappingFlag = Cast<AFlag>(OtherActor);
	if (OverlappingFlag)
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlapping with a flag"));
	}

	if (OverlappingFlag && OverlappingFlag->GetTeam() != Team)
	{
		UE_LOG(LogTemp, Warning, TEXT("Overlapping with right flag"));
		ACaptureTheFlagGameMode* GameMode = Cast<ACaptureTheFlagGameMode>(GetWorld()->GetAuthGameMode());
		if (GameMode)
		{
			GameMode->FlagCaptured(OverlappingFlag, this);
			UE_LOG(LogTemp, Warning, TEXT("Gamemode OK"));
		}
		OverlappingFlag->ResetFlag();
	}
}

