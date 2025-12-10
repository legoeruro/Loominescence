#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TooltipTypes.generated.h"

USTRUCT(BlueprintType)
struct FTooltipData
{
	GENERATED_BODY()

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString RichText;
};

UINTERFACE(BlueprintType)
class UTooltipTypes : public UInterface
{
	GENERATED_BODY()
};

// Implement this on actors that can provide a tooltip.
// Tooltip text will be changed by the crafthand class (upon collision).
class ITooltipTypes
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category="Tooltip")
	FTooltipData GetTooltipData() const;
};
