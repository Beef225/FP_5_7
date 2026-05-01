// Copyright JG

#include "UI/Widget/Inventory/Composite/FP_SkillLeaf_Name.h"
#include "Components/RichTextBlock.h"

void UFP_SkillLeaf_Name::PopulateSkillData(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS)
{
	if (Entry.SkillName.IsNone())
	{
		Collapse();
		return;
	}

	RichText_SkillName->SetText(FText::FromName(Entry.SkillName));
	Expand();
}
