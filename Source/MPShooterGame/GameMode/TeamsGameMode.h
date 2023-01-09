// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BlasterGameMode.h"
#include "TeamsGameMode.generated.h"

/**
 * 
 */
UCLASS()
class MPSHOOTERGAME_API ATeamsGameMode : public ABlasterGameMode
{
	GENERATED_BODY()

public:

	ATeamsGameMode();

	virtual void PostLogin(APlayerController* NewPlayer) override;
	virtual void Logout(AController* Exiting) override;
	virtual float CalculateDamage(AController* Attacker, AController* Victim, float BaseDamage) override;
	virtual void PlayerEliminated(class ABlasterCharacter* ElimmedCharacter, class ABlasterPlayerController* VictimController, ABlasterPlayerController* AttackerController) override;


protected:

	virtual void HandleMatchHasStarted() override;

private:

	UPROPERTY(EditAnywhere)
	bool bAllowFriendlyFire{ true };

public:

	FORCEINLINE void SetFriendlyFire(bool bFriendlyFireEnabled) { bAllowFriendlyFire = bAllowFriendlyFire; }
	
};
