#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "TooltipTypes.h"
#include "TooltipProvider.generated.h"

UINTERFACE(BlueprintType)
class UTooltipProvider : public UInterface
{
	GENERATED_BODY()
};

class ITooltipProvider
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	FTooltipData GetTooltipData() const;
};
