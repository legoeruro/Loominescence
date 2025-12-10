#include "MixingManager.h"

FPotionData UPotionMixingManager::GetResult(EElementalType A, EElementalType B) const
{
	FMixRecipe recipe = GetRecipe(A, B);
	return recipe.ResultPotion;
}

FMixRecipe UPotionMixingManager::GetRecipe(EElementalType A, EElementalType B) const
{
	for (const FMixRecipe& Recipe : Recipes)
	{
		UE_LOG(LogTemp, Warning, TEXT("Recipe %hhd %hhd"), Recipe.ElementA, Recipe.ElementB);
		if ((Recipe.ElementA == A && Recipe.ElementB == B) ||
			(Recipe.ElementA == B && Recipe.ElementB == A))
		{
			return Recipe;
		}
	}

	FPotionData EmptyPotion;
	EmptyPotion.PotionName = "Mysterious potion";
	EmptyPotion.LiquidColor = FLinearColor(
		FMath::FRand(),    
		FMath::FRand(),    
		FMath::FRand(),
		1.0f           
	);
	EmptyPotion.EffectType = EPotionEffect::None;

	FMixRecipe FMixRecipe;
	FMixRecipe.ElementA = A;
	FMixRecipe.ElementB = B;
	FMixRecipe.ResultPotion = EmptyPotion;
	return FMixRecipe;
}