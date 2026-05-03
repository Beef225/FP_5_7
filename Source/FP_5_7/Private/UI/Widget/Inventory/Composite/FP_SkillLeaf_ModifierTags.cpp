// Copyright JG

#include "UI/Widget/Inventory/Composite/FP_SkillLeaf_ModifierTags.h"
#include "Components/RichTextBlock.h"

void UFP_SkillLeaf_ModifierTags::PopulateSkillData(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS)
{
	if (Entry.SkillModifierTags.IsEmpty())
	{
		Collapse();
		return;
	}

	TArray<FString> Parts;
	for (const FGameplayTag& Tag : Entry.SkillModifierTags)
	{
		FString Right;
		Tag.GetTagName().ToString().Split(TEXT("."), nullptr, &Right, ESearchCase::IgnoreCase, ESearchDir::FromEnd);
		if (!Right.IsEmpty())
			Parts.Add(Right);
	}

	if (Parts.IsEmpty())
	{
		Collapse();
		return;
	}

	RichText_ModifierTags->SetText(FText::FromString(FString::Join(Parts, TEXT(", "))));
	Expand();
}
