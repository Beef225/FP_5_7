// Copyright JG

#include "UI/Widget/Inventory/Composite/FP_SkillLeaf_SkillSpeed.h"
#include "UI/Widget/Inventory/Composite/FP_SkillStatCalc.h"
#include "Components/TextBlock.h"

void UFP_SkillLeaf_SkillSpeed::PopulateSkillData(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS)
{
	const float Speed = FP_SkillStatCalc::CalcEffectiveSpeed(Entry, PS);
	if (Speed <= 0.f)
	{
		Collapse();
		return;
	}

	Text_SkillSpeed->SetText(FText::FromString(FString::Printf(TEXT("%.2f\u00D7"), Speed)));
	Expand();
}
