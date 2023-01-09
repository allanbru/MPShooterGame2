// Fill out your copyright notice in the Description page of Project Settings.


#include "Flag.h"
#include "Components/SphereComponent.h"
#include "Components/SkeletalMeshComponent.h"
#include "Components/WidgetComponent.h"

AFlag::AFlag()
{
	FlagMesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Flag Mesh"));
	SetRootComponent(FlagMesh);

	GetAreaSphere()->SetupAttachment(FlagMesh);
	GetPickupWidget()->SetupAttachment(FlagMesh);
}

