// Copyright JG

#include "UI/Widget/Inventory/Composite/FP_SkillLeaf_DPS.h"
#include "UI/Widget/Inventory/Composite/FP_SkillStatCalc.h"
#include "Components/TextBlock.h"

void UFP_SkillLeaf_DPS::PopulateSkillData(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS)
{
	const float DPS = FP_SkillStatCalc::CalcDPS(Entry, PS);
	if (DPS <= 0.f)
	{
		Collapse();
		return;
	}

	Text_DPS->SetText(FText::FromString(FString::Printf(TEXT("%.0f"), DPS)));
	Expand();
}
