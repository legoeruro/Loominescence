#include "TooltipWidget.h"
#include "Components/RichTextBlock.h"

void UTooltipWidgetBase::UpdateFromTooltipData(const FTooltipData& Data)
{
	if (TooltipTextBlock)
	{
		TooltipTextBlock->SetText(FText::FromString(Data.RichText));
	}

	// For animations or sound effects
	OnTooltipShown();
}
