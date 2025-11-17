#include "MixingManager.h"

FPotionData UPotionMixingManager::GetResult(EElementalType A, EElementalType B) const
{
	for (const FMixRecipe& Recipe : Recipes)
	{
		if ((Recipe.ElementA == A && Recipe.ElementB == B) ||
			(Recipe.ElementA == B && Recipe.ElementB == A))
		{
			return Recipe.ResultPotion;
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
	return EmptyPotion;
}