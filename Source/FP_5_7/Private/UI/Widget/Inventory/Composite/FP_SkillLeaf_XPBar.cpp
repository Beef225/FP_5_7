// Copyright JG

#include "UI/Widget/Inventory/Composite/FP_SkillLeaf_XPBar.h"
#include "AbilitySystem/Data/FP_SkillLevelUpInfo.h"
#include "Components/ProgressBar.h"
#include "Components/TextBlock.h"
#include "Player/FP_PlayerState.h"

void UFP_SkillLeaf_XPBar::PopulateSkillData(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS)
{
	const UFP_SkillLevelUpInfo* Curve = Entry.SkillLevelUpInfo;
	if (!Curve || Entry.MaxLevel <= 1)
	{
		Collapse();
		return;
	}

	const int32 CurrentXP    = PS ? PS->GetSkillXP(Entry.SkillTag) : 0;
	const int32 MaxLevel     = FMath::Min(Entry.MaxLevel, Curve->GetMaxLevel());
	const int32 CurrentLevel = FMath::Min(Curve->FindLevelForXP(CurrentXP), MaxLevel);

	if (CurrentLevel >= MaxLevel)
	{
		ProgressBar_XP->SetPercent(1.f);
		Text_XPCurrent->SetText(FText::AsNumber(CurrentXP));
		Text_XPNext->SetText(FText::FromString(TEXT("MAX")));
	}
	else
	{
		const int32 ThisReq = Curve->GetXPRequirementForLevel(CurrentLevel);
		const int32 NextReq = Curve->GetXPRequirementForLevel(CurrentLevel + 1);
		const int32 Delta   = NextReq - ThisReq;
		const float Percent = (Delta > 0)
			? FMath::Clamp(static_cast<float>(CurrentXP - ThisReq) / static_cast<float>(Delta), 0.f, 1.f)
			: 1.f;

		ProgressBar_XP->SetPercent(Percent);
		Text_XPCurrent->SetText(FText::AsNumber(CurrentXP - ThisReq));
		Text_XPNext->SetText(FText::AsNumber(Delta));
	}

	Expand();
}
