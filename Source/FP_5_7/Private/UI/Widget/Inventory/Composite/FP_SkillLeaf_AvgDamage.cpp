// Copyright JG

#include "UI/Widget/Inventory/Composite/FP_SkillLeaf_AvgDamage.h"
#include "UI/Widget/Inventory/Composite/FP_SkillStatCalc.h"
#include "Components/TextBlock.h"

void UFP_SkillLeaf_AvgDamage::PopulateSkillData(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS)
{
	const float Damage = FP_SkillStatCalc::CalcAvgDamage(Entry, PS);
	if (Damage <= 0.f)
	{
		Collapse();
		return;
	}

	Text_AvgDamage->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), Damage)));
	Expand();
}
