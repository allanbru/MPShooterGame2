// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "BlasterHUD.generated.h"

USTRUCT(BlueprintType)
struct FHUDPackage {
	GENERATED_BODY()

public:
	UPROPERTY()
	class UTexture2D* CrosshairsCenter;
	
	UPROPERTY()
	UTexture2D* CrosshairsLeft;
	
	UPROPERTY()
	UTexture2D* CrosshairsRight;
	
	UPROPERTY()
	UTexture2D* CrosshairsTop;
	
	UPROPERTY()
	UTexture2D* CrosshairsBottom;
	
	float CrosshairSpread;
	
	FLinearColor CrosshairsColor;
};
/**
 * 
 */
UCLASS()
class MPSHOOTERGAME_API ABlasterHUD : public AHUD
{
	GENERATED_BODY()

public:

	virtual void DrawHUD() override;

	UPROPERTY(EditAnywhere, Category = "Player Stats")
	TSubclassOf<class UUserWidget> CharacterOverlayClass;

	UPROPERTY()
	class UCharacterOverlay* CharacterOverlay{nullptr};
	void AddCharacterOverlay();

	UPROPERTY(EditAnywhere, Category = "Announcements")
	TSubclassOf<UUserWidget> AnnouncementClass;

	UPROPERTY(EditAnywhere, Category = "Class Selection")
	TSubclassOf<UUserWidget> ClassSelectorWidgetClass;
	UUserWidget* ClassSelectorWidget{ nullptr };
	bool bClassSelectorOnScreen{ false };
	void ToggleClassSelectorWidget();

	UPROPERTY()
	class UAnnouncement* Announcement{ nullptr };
	void AddAnnouncement();

	void AddElimAnnouncement(FString Attacker, FString Victim);

protected:

	virtual void BeginPlay() override;
	
private:

	UPROPERTY()
	class APlayerController* OwningPlayer;
	
	FHUDPackage HUDPackage;

	void DrawCrosshair(UTexture2D* Texture, FVector2D ViewportCenter, FVector2D Spread, FLinearColor CrosshairsColor);

	UPROPERTY(EditAnywhere)
	float CrosshairSpreadMax = 16.f;

	UPROPERTY(EditAnywhere, Category = "Announcements")
	TSubclassOf<class UElimAnnouncement> ElimAnnouncementClass;

	UPROPERTY(EditAnywhere)
	float ElimAnnouncementTime{ 3.f };

	UFUNCTION()
	void ElimAnnouncementTimerFinished(UElimAnnouncement* MsgToRemove);

	UPROPERTY()
	TArray<UElimAnnouncement*> ElimMessages;

public:
	
	FORCEINLINE void SetHUDPackage(const FHUDPackage& Package) { HUDPackage = Package; };

};
