#pragma once

#include "CoreMinimal.h"
#include "GameTypes.generated.h"

UENUM(BlueprintType)
enum class EElementalType : uint8
{
	None UMETA(Display = "None"),
	Water UMETA(Display = "Water"),
	Mineral UMETA(Display = "Mineral"),
	Heat UMETA(Display = "Heat"),
	Air UMETA(Display = "Air"),
	Light UMETA(Display = "Light"),
	Shadow UMETA(Display = "Shadow"),
	Organic UMETA(Display = "Organic"),
	Poison UMETA(Display = "Poison"),
	Cleanse UMETA(Display = "Cleanse"),
};

UENUM(BlueprintType)
enum class EPotionEffect : uint8
{
	None UMETA(Display = "None"),
	FloatUp UMETA(Display = "Float Up"),
	IncreaseSpeed UMETA(Display = "Increase Speed"),
	Explode UMETA(Display = "Explode"),
	DoubleAttack UMETA(Display = "Double Attack"),
	Heal UMETA(Display = "Heal"),
};

USTRUCT(BlueprintType)
struct FPotionData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Potion")
	FName PotionName;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Potion")
	FString PotionDescription;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Potion")
	FLinearColor LiquidColor;

	// Icon / sprite for UI
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Potion")
	UTexture2D* PotionIcon;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Potion")
	EPotionEffect EffectType;

	// UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Potion")
	// float EffectPower = 1.0f;

	// TODO: think of defining duration, intensity, etc. dynamically
};
