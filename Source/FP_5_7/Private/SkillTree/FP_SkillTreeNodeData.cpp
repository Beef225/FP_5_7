// Copyright JG

#include "SkillTree/FP_SkillTreeNodeData.h"

void UFP_SkillTreeNodeData::AllocateEffects(APlayerController* PC)
{
	for (TInstancedStruct<FFP_SkillTreeNodeEffect>& Inst : Effects)
	{
		if (FFP_SkillTreeNodeEffect* Effect = Inst.GetMutablePtr<FFP_SkillTreeNodeEffect>())
			Effect->OnAllocate(PC);
	}
}

void UFP_SkillTreeNodeData::DeallocateEffects(APlayerController* PC)
{
	for (TInstancedStruct<FFP_SkillTreeNodeEffect>& Inst : Effects)
	{
		if (FFP_SkillTreeNodeEffect* Effect = Inst.GetMutablePtr<FFP_SkillTreeNodeEffect>())
			Effect->OnDeallocate(PC);
	}
}
