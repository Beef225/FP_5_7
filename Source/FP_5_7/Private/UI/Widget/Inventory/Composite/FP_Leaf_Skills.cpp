// Copyright JG

#include "UI/Widget/Inventory/Composite/FP_Leaf_Skills.h"
#include "UI/Widget/Inventory/Composite/FP_SkillDisplay.h"
#include "AbilitySystem/Data/FP_SkillLibrary.h"
#include "Components/VerticalBox.h"
#include "Player/FP_PlayerState.h"

void UFP_Leaf_Skills::SetSkills(const TArray<FGameplayTag>& SkillTags, const AFP_PlayerState* PS, const UFP_SkillLibrary* Library)
{
	Box_Skills->ClearChildren();

	if (!Library || !SkillDisplayClass) return;

	for (const FGameplayTag& Tag : SkillTags)
	{
		const FFP_AbilityEntry Entry = Library->FindAbilityEntryForTag(Tag, true);
		if (!Entry.SkillTag.IsValid()) continue;

		UFP_SkillDisplay* Display = CreateWidget<UFP_SkillDisplay>(GetOwningPlayer(), SkillDisplayClass);
		if (!IsValid(Display)) continue;

		const int32 CurrentXP = PS ? PS->GetSkillXP(Tag) : 0;
		Display->Populate(Entry, CurrentXP);
		Box_Skills->AddChildToVerticalBox(Display);
	}
}
