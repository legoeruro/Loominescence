#include "LoomiUtils.h"
#include "GameTypes.h"

bool LoomiUtils::IsElementInInventory(
    UActorComponent* InventoryComp,
    const EElementalType ItemElementToFind)
{
    if (!InventoryComp)
        return false;

    UFunction* Func = InventoryComp->FindFunction(TEXT("IsElementInInventory"));
    if (!Func)
    {
        UE_LOG(LogTemp, Error, TEXT("IsElementInInventory not found"));
        return false;
    }

    struct FHasElementParams
    {
        EElementalType Type;
        bool ReturnValue;
    };

    FHasElementParams Params;
    Params.Type = ItemElementToFind;
    Params.ReturnValue = false;

    InventoryComp->ProcessEvent(Func, &Params);

    return Params.ReturnValue;
    
    // OutIndex = -1;
    //
    // if (!InventoryComp) return false;
    //
    // // Find GetItems()
    // UE_LOG(LogTemp, Warning, TEXT("Looking for blueprint function"));
    //
    // UFunction* GetItemsFunc = InventoryComp->FindFunction(TEXT("IsElementInInventory"));
    // if (!GetItemsFunc)
    // {
    //     UE_LOG(LogTemp, Error, TEXT("Blueprint function GetItems() not found!"));
    //     return false;
    // }
    //
    //     UE_LOG(LogTemp, Warning, TEXT("Blueprint function GetItems() found!"));
    //
    // // Allocate space for return value
    // void* ReturnValueAddr = FMemory::Malloc(GetItemsFunc->ParmsSize);
    // FMemory::Memzero(ReturnValueAddr, GetItemsFunc->ParmsSize);
    //
    // InventoryComp->ProcessEvent(GetItemsFunc, ReturnValueAddr);
    //
    // FProperty* RetProp = GetItemsFunc->GetReturnProperty();
    // UE_LOG(LogTemp, Error, TEXT("Return property class = %s"), 
    //     RetProp ? *RetProp->GetClass()->GetName() : TEXT("NULL"));
    //
    // UE_LOG(LogTemp, Error, TEXT("InventoryComp class = %s"), *InventoryComp->GetClass()->GetName());
    //     
    // UE_LOG(LogTemp, Warning, TEXT("ParmsSize = %d"), GetItemsFunc->ParmsSize);
    //
    // // Extract return property
    // FArrayProperty* ArrayProp = CastField<FArrayProperty>(GetItemsFunc->GetReturnProperty());
    // if (!ArrayProp)
    // {
    //     FMemory::Free(ReturnValueAddr);
    //     UE_LOG(LogTemp, Error, TEXT("No array prop found!"));
    //     return false;
    // }
    //
    // // Create array helper to iterate items
    // FScriptArrayHelper ArrayHelper(ArrayProp, ReturnValueAddr);
    //
    // // Struct type of S_Item
    // FStructProperty* StructProp = CastField<FStructProperty>(ArrayProp->Inner);
    // UScriptStruct* StructType = StructProp->Struct;
    //
    // UE_LOG(LogTemp, Warning, TEXT("==== Dumping struct properties for %s ===="),
    // *StructType->GetName());
    //
    // for (TFieldIterator<FProperty> It(StructType); It; ++It)
    // {
    //     FProperty* P = *It;
    //
    //     UE_LOG(LogTemp, Warning, TEXT("Property: %s   (Type: %s)"),
    //         *P->GetName(),
    //         *P->GetClass()->GetName());
    // }
    //
    // UE_LOG(LogTemp, Error, TEXT("Array length: %d"), ArrayHelper.Num());
    //
    // // Due to compilations names of the variables will vary accordingly, with the name of the property in the beginning
    // // TODO: find a less hacky way to obtain this value
    // FProperty* NameProp = nullptr;
    // FProperty* ElementProp = nullptr;
    // for (TFieldIterator<FProperty> It(StructType); It; ++It)
    // {
    //     FProperty* P = *It;
    //     const FString PName = P->GetName();
    //
    //     if (!NameProp && PName.StartsWith(TEXT("ItemName")))
    //     {
    //         NameProp = P;
    //     }
    //     else if (!ElementProp && PName.StartsWith(TEXT("ItemElement")))
    //     {
    //         ElementProp = P;
    //     }
    // }
    //
    // if (!NameProp)
    // {
    //     UE_LOG(LogTemp, Error, TEXT("Could not find ItemName* property on struct %s"), *StructType->GetName());
    // }
    // if (!ElementProp)
    // {
    //     UE_LOG(LogTemp, Error, TEXT("Could not find ItemElement* property on struct %s"), *StructType->GetName());
    // }
    //
    // UE_LOG(LogTemp, Warning, TEXT("==== Inventory Debug: %d items ===="), ArrayHelper.Num());
    //
    // FEnumProperty* EnumProp = CastField<FEnumProperty>(ElementProp);
    // FByteProperty* ByteProp = CastField<FByteProperty>(ElementProp);
    // // Loop through items
    // for (int32 i = 0; i < ArrayHelper.Num(); i++)
    // {
    //     void* ElementPtr = ArrayHelper.GetRawPtr(i);
    //
    //     FString OutText;
    //     StructType->ExportText(OutText, ElementPtr, nullptr, nullptr, PPF_None, nullptr);
    //     UE_LOG(LogTemp, Warning, TEXT("STRUCT DUMP [%d]: %s"), i, *OutText);
    //
    //     EElementalType ElementToFind = ItemElementToFind;
    //     EElementalType ElementalValue = EElementalType::None;
    //     // if (EnumProp)
    //     // {
    //     //     FEnumProperty* EnumProp = CastField<FEnumProperty>(ElementProp);
    //     //     UEnum* EnumDef = EnumProp->GetEnum();
    //     //
    //     //     FString EnumName = EnumDef->GetNameStringByValue(Raw);
    //     //
    //     //     UE_LOG(LogTemp, Warning,
    //     //         TEXT("Item[%d] Element raw value = %llu (underlying property = %s)"),
    //     //         i,
    //     //         Raw,
    //     //         *Num->GetClass()->GetName());
    //     //     
    //     //     ElementalValue = static_cast<EElementalType>(Raw);
    //     // }
    //     if (EnumProp)
    //     {
    //         // Get the underlying numeric property (always uint8 or uint32)
    //         FNumericProperty* Num = CastField<FNumericProperty>(EnumProp->GetUnderlyingProperty());
    //         if (!Num)
    //         {
    //             UE_LOG(LogTemp, Error, TEXT("EnumProp has no underlying numeric property!"));
    //             return false;
    //         }
    //
    //         uint64 Raw = Num->GetUnsignedIntPropertyValue(ElementPtr);
    //
    //         UEnum* Enum = EnumProp->GetEnum();
    //
    //         for (int32 j = 0; j < Enum->NumEnums(); j++)
    //         {
    //             UE_LOG(LogTemp, Warning, TEXT("Enum index %d = %s (value = %lld)"),
    //                 j,
    //                 *Enum->GetDisplayNameTextByIndex(i).ToString(),
    //                 Enum->GetValueByIndex(i)
    //             );
    //         }
    //
    //
    //         UE_LOG(LogTemp, Warning, TEXT("Raw enum value = %llu"), Raw);
    //
    //         UEnum* EnumDef = EnumProp->GetEnum();
    //         if (!EnumDef)
    //         {
    //             UE_LOG(LogTemp, Error, TEXT("EnumProp->GetEnum() returned null!"));
    //             return false;
    //         }
    //
    //         FString ValueString = EnumDef->GetNameStringByValue(Raw);
    //
    //         UE_LOG(LogTemp, Warning, TEXT("Blueprint enum resolved this to: %s"), *ValueString);
    //
    //         if (ValueString.Equals(TEXT("Light"), ESearchCase::IgnoreCase))
    //         {
    //             UE_LOG(LogTemp, Warning, TEXT("Item IS LIGHT!"));
    //         }
    //     }
    //     else
    //     {
    //         UE_LOG(LogTemp, Error, TEXT("Property is not an enum!"));
    //     }
    //     UE_LOG(LogTemp, Warning, TEXT("Comparing element %d and %d!"), ElementToFind, ElementalValue);
    //     
    //
    //     if (ElementToFind == ElementalValue)
    //     {
    //         OutIndex = i;
    //         FMemory::Free(ReturnValueAddr);
    //         return true;
    //     }
    // }
    //
    // FMemory::Free(ReturnValueAddr);
    // return false;
}

UPotionMixingManager* LoomiUtils::GetMixingManager(UObject* WorldContext)
{
    UObject* GI = WorldContext->GetWorld()->GetGameInstance();

    if (!WorldContext)
    {
        UE_LOG(LogTemp, Error, TEXT("GetMixingManager: No WorldContext!"));
        return nullptr;
    }

    if (!GI)
    {
        UE_LOG(LogTemp, Error, TEXT("GetMixingManager: No GameInstance!"));
        return nullptr;
    }

    // Reflection access for BP variable "MixingManager"
    FProperty* Prop = GI->GetClass()->FindPropertyByName(TEXT("MixingManager"));
    if (!Prop)
    {
        UE_LOG(LogTemp, Error, TEXT("MixingManager variable not found in GameInstance!"));
        return nullptr;
    }

    FObjectProperty* ObjProp = CastField<FObjectProperty>(Prop);
    if (!ObjProp)
    {
        UE_LOG(LogTemp, Error, TEXT("MixingManager is not an object property!"));
        return nullptr;
    }

    UPotionMixingManager* Result =
        Cast<UPotionMixingManager>(ObjProp->GetObjectPropertyValue_InContainer(GI));

    if (!Result)
    {
        UE_LOG(LogTemp, Error, TEXT("MixingManager is null — did you assign it in BP_GameInstance?"));
    }

    return Result;
}
