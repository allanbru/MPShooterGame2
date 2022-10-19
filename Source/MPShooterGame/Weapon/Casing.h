// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Casing.generated.h"

UCLASS()
class MPSHOOTERGAME_API ACasing : public AActor
{
	GENERATED_BODY()
	
public:	
	ACasing();

protected:
	virtual void BeginPlay() override;

	UFUNCTION()
	virtual void OnHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, FVector NormalImpulse, const FHitResult& Hit);

private:

	UPROPERTY(EditAnywhere)
	float ShellLifeSpan{3.f};

	UPROPERTY(VisibleAnywhere)
	UStaticMeshComponent* CasingMesh{ nullptr };

	UPROPERTY(EditAnywhere)
	float ShellEjectionImpulse{ 10.f };

	UPROPERTY(EditAnywhere)
	class USoundCue* ShellSound{ nullptr };

public:	

};
