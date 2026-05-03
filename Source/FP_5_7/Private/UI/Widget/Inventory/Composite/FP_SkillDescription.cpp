// Copyright JG

#include "UI/Widget/Inventory/Composite/FP_SkillDescription.h"
#include "UI/Widget/Inventory/Composite/FP_SkillLeafBase.h"

void UFP_SkillDescription::Populate(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS)
{
	ApplyFunction([&Entry, PS](UFP_CompositeBase* Widget)
	{
		if (UFP_SkillLeafBase* Leaf = Cast<UFP_SkillLeafBase>(Widget))
			Leaf->PopulateSkillData(Entry, PS);
	});
}
