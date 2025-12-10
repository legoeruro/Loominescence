#pragma once

#include "CoreMinimal.h"
#include "GameTypes.h"
#include "MixingManager.h"
#include "ULoomiUtils.generated.h"

UCLASS()
class LOOMINESCENCE_API ULoomiUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public:
    // Assumes the inventory is in an array. Assumes "InventoryComp" has a function GetItems().
    // Uses reflection to get blueprint data into C++
	static bool IsElementInInventory(UActorComponent* InventoryComp, const EElementalType ItemElementToFind);

	static UPotionMixingManager* GetMixingManager(UObject* WorldContext);

	static UActorComponent* GetInventoryComponent(UObject* WorldContext);
};
