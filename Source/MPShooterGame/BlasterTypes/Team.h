#pragma once

UENUM(BlueprintType)
enum class ETeam : uint8 
{
	ET_Red UMETA(DisplayName = "RedTeam"),
	ET_Blue UMETA(DisplayName = "BlueTeam"),
	ET_NoTeam UMETA(DisplayName = "NoTeam"),

	ET_MAX UMETA(DisplayName = "DefaultMAX")
};