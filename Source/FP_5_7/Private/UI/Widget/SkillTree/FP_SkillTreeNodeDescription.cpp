// Copyright JG

#include "UI/Widget/SkillTree/FP_SkillTreeNodeDescription.h"
#include "UI/Widget/SkillTree/FP_SkillTreeLeaf_Header.h"
#include "UI/Widget/SkillTree/FP_SkillTreeLeaf_NodeDesc.h"
#include "SkillTree/FP_SkillTreeNodeData.h"

void UFP_SkillTreeNodeDescription::Populate(const UFP_SkillTreeNodeData* Data)
{
	if (!Data) return;

	ApplyFunction([Data](UFP_CompositeBase* Widget)
	{
		if (UFP_SkillTreeLeaf_Header* Header = Cast<UFP_SkillTreeLeaf_Header>(Widget))
			Header->SetNodeData(Data);
		else if (UFP_SkillTreeLeaf_NodeDesc* Desc = Cast<UFP_SkillTreeLeaf_NodeDesc>(Widget))
			Desc->SetDescription(Data->NodeDescription);
	});
}

