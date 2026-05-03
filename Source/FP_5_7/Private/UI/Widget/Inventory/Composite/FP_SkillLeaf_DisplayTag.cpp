// Copyright JG

#include "UI/Widget/Inventory/Composite/FP_SkillLeaf_DisplayTag.h"
#include "Components/RichTextBlock.h"

void UFP_SkillLeaf_DisplayTag::PopulateSkillData(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS)
{
	if (Entry.DisplayTag.IsEmpty())
	{
		Collapse();
		return;
	}

	RichText_DisplayTag->SetText(Entry.DisplayTag);
	Expand();
}
