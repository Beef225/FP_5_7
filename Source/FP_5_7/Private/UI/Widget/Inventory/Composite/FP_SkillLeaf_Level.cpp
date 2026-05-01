// Copyright JG

#include "UI/Widget/Inventory/Composite/FP_SkillLeaf_Level.h"
#include "AbilitySystem/Data/FP_SkillLevelUpInfo.h"
#include "Components/TextBlock.h"
#include "Player/FP_PlayerState.h"

void UFP_SkillLeaf_Level::PopulateSkillData(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS)
{
	if (Entry.MaxLevel <= 1)
	{
		Collapse();
		return;
	}

	const int32 CurrentXP    = PS ? PS->GetSkillXP(Entry.SkillTag) : 0;
	const UFP_SkillLevelUpInfo* Curve = Entry.SkillLevelUpInfo;

	const int32 MaxLevel     = Curve ? FMath::Min(Entry.MaxLevel, Curve->GetMaxLevel()) : Entry.MaxLevel;
	const int32 CurrentLevel = Curve ? FMath::Min(Curve->FindLevelForXP(CurrentXP), MaxLevel) : 1;

	Text_Level->SetText(FText::FromString(FString::Printf(TEXT("LVL %d"), CurrentLevel)));

	if (CurrentLevel >= MaxLevel)
		Text_MaxLevel->SetText(FText::FromString(TEXT("MAX")));
	else
		Text_MaxLevel->SetText(FText::FromString(FString::Printf(TEXT("/ %d"), MaxLevel)));

	Expand();
}
