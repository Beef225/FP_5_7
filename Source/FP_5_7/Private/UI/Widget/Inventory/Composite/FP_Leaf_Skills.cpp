// Copyright JG

#include "UI/Widget/Inventory/Composite/FP_Leaf_Skills.h"
#include "UI/Widget/Inventory/Composite/FP_SkillDisplay.h"
#include "AbilitySystem/Data/FP_SkillLibrary.h"
#include "Components/VerticalBox.h"

void UFP_Leaf_Skills::SetSkills(const TArray<FGameplayTag>& SkillTags, const TArray<int32>& Levels, const UFP_SkillLibrary* Library)
{
	Box_Skills->ClearChildren();

	if (!Library || !SkillDisplayClass) return;

	for (int32 i = 0; i < SkillTags.Num(); ++i)
	{
		const FFP_AbilityEntry Entry = Library->FindAbilityEntryForTag(SkillTags[i], true);
		if (!Entry.SkillTag.IsValid()) continue;

		UFP_SkillDisplay* Display = CreateWidget<UFP_SkillDisplay>(GetOwningPlayer(), SkillDisplayClass);
		if (!IsValid(Display)) continue;

		const int32 Level = Levels.IsValidIndex(i) ? Levels[i] : 1;
		Display->Populate(Entry, Level);
		Box_Skills->AddChildToVerticalBox(Display);
	}
}
