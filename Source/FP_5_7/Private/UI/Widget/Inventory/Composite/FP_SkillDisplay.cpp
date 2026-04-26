// Copyright JG

#include "UI/Widget/Inventory/Composite/FP_SkillDisplay.h"
#include "Components/Image.h"
#include "Components/RichTextBlock.h"
#include "Components/TextBlock.h"
#include "Engine/Texture2D.h"

void UFP_SkillDisplay::Populate(const FFP_AbilityEntry& Entry, int32 Level)
{
	if (Entry.SkillIcon)
	{
		FSlateBrush Brush;
		Brush.SetResourceObject(const_cast<UTexture2D*>(Entry.SkillIcon.Get()));
		Brush.DrawAs = ESlateBrushDrawType::Image;
		Image_SkillIcon->SetBrush(Brush);
	}

	RichText_SkillName->SetText(FText::FromName(Entry.SkillName));

	Text_SkillLevel->SetText(FText::FromString(FString::Printf(TEXT("LVL %d"), Level)));
}
