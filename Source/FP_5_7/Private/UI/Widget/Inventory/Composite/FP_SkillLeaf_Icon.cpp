// Copyright JG

#include "UI/Widget/Inventory/Composite/FP_SkillLeaf_Icon.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void UFP_SkillLeaf_Icon::PopulateSkillData(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS)
{
	if (!Entry.SkillIcon)
	{
		Collapse();
		return;
	}

	Image_SkillIcon->SetBrushFromTexture(const_cast<UTexture2D*>(Entry.SkillIcon.Get()));
	Expand();
}
