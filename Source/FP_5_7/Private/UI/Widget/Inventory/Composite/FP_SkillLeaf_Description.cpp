// Copyright JG

#include "UI/Widget/Inventory/Composite/FP_SkillLeaf_Description.h"
#include "Components/RichTextBlock.h"

void UFP_SkillLeaf_Description::PopulateSkillData(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS)
{
	if (Entry.SkillDescription.IsEmpty())
	{
		Collapse();
		return;
	}

	RichText_Description->SetText(Entry.SkillDescription);
	Expand();
}
