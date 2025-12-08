#include "LoomiUtils.h"

bool LoomiUtils::FindItemInInventory(UActorComponent* InventoryComp, const FString& ItemNameToFind, int32& OutIndex)
{
    OutIndex = -1;

    if (!InventoryComp) return false;

    // Find GetItems()
    UFunction* GetItemsFunc = InventoryComp->FindFunction(TEXT("GetItems"));
    if (!GetItemsFunc)
    {
        UE_LOG(LogTemp, Error, TEXT("Blueprint function GetItems() not found!"));
        return false;
    }

    // Allocate space for return value
    void* ReturnValueAddr = FMemory::Malloc(GetItemsFunc->ParmsSize);
    FMemory::Memzero(ReturnValueAddr, GetItemsFunc->ParmsSize);

    InventoryComp->ProcessEvent(GetItemsFunc, ReturnValueAddr);
    
    // Extract return property
    FArrayProperty* ArrayProp = CastField<FArrayProperty>(GetItemsFunc->GetReturnProperty());
    if (!ArrayProp)
    {
        FMemory::Free(ReturnValueAddr);
        return false;
    }

    // Create array helper to iterate items
    FScriptArrayHelper ArrayHelper(ArrayProp, ReturnValueAddr);

    // Struct type of S_Item
    FStructProperty* StructProp = CastField<FStructProperty>(ArrayProp->Inner);
    UScriptStruct* StructType = StructProp->Struct;

    // Find "ItemName"
    FProperty* NameProp = StructType->FindPropertyByName(TEXT("ItemName"));

    // Loop through items
    for (int32 i = 0; i < ArrayHelper.Num(); i++)
    {
        void* ElementPtr = ArrayHelper.GetRawPtr(i);

        // Read ItemName
        FText ItemName;
        if (FTextProperty* TextProp = CastField<FTextProperty>(NameProp))
        {
            ItemName = TextProp->GetPropertyValue_InContainer(ElementPtr);
        }

        if (ItemName.ToString().Equals(ItemNameToFind, ESearchCase::IgnoreCase))
        {
            OutIndex = i;
            FMemory::Free(ReturnValueAddr);
            return true;
        }
    }

    FMemory::Free(ReturnValueAddr);
    return false;
}
