 #pragma once

// #include "LoomiUtils.generated.h"

class LoomiUtils
{
public:
    // Assumes the inventory is in an array. Assumes "InventoryComp" has a function GetItems().
    // Uses reflection to get blueprint data into C++
    static bool FindItemInInventory(UActorComponent* InventoryComp, const FString& ItemNameToFind, int32& OutIndex);
};
