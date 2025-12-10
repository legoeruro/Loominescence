#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "TooltipTypes.h"
#include "TooltipWidget.generated.h"

UCLASS()
class LOOMINESCENCE_API UTooltipWidgetBase : public UUserWidget
{
	GENERATED_BODY()

public:

	// Bind your RichTextBlock to this variable (in UMG)
	UPROPERTY(meta=(BindWidget))
	class URichTextBlock* TooltipTextBlock;

	// Called by PlayerController when tooltip should update
	UFUNCTION(BlueprintCallable)
	void UpdateFromTooltipData(const FTooltipData& Data);
    
	// Optional padding or layout control
	UFUNCTION(BlueprintImplementableEvent)
	void OnTooltipShown();
};

