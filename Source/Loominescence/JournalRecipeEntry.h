#pragma once

#include "CoreMinimal.h"
#include "Utils/GameTypes.h"
#include "Utils/MixingManager.h"
#include "JournalRecipeEntry.generated.h"

USTRUCT(BlueprintType)
struct FJournalRecipeEntry
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FMixRecipe Recipe;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool bIsNew = false;
};