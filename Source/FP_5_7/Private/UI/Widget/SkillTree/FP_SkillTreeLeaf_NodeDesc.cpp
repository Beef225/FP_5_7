// Copyright JG

#include "UI/Widget/SkillTree/FP_SkillTreeLeaf_NodeDesc.h"
#include "Components/TextBlock.h"

void UFP_SkillTreeLeaf_NodeDesc::SetDescription(const FText& Desc)
{
	if (Desc.IsEmpty())
	{
		Collapse();
		return;
	}

	Text_Description->SetText(Desc);
	Expand();
}
