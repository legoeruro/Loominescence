#pragma once

#include "CoreMinimal.h"
#include "Utils/GameTypes.h"
#include "PotionActor.h"
#include "MixingManager.generated.h"

USTRUCT(BlueprintType)
struct FMixRecipe
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	EElementalType ElementA;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	EElementalType ElementB;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Recipe")
	FPotionData ResultPotion;
};

UCLASS(Blueprintable, BlueprintType)
class LOOMINESCENCE_API UPotionMixingManager : public UDataAsset
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Mixing")
	TArray<FMixRecipe> Recipes;

	UFUNCTION(BlueprintCallable, Category = "Mixing")
	FPotionData GetResult(EElementalType A, EElementalType B) const;
};
