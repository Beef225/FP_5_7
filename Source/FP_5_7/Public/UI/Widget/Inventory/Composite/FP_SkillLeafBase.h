// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Inventory/Composite/FP_Leaf.h"
#include "AbilitySystem/Data/FP_SkillLibrary.h"
#include "FP_SkillLeafBase.generated.h"

class AFP_PlayerState;

/**
 * Abstract base for all skill description leaf widgets.
 *
 * Concrete leaves override PopulateSkillData to display a specific aspect of the skill
 * (name, icon, XP bar, description text, stats, etc.).
 * Each leaf is responsible for calling Expand() / Collapse() on itself depending on
 * whether it has valid data to show.
 */
UCLASS(Abstract)
class FP_5_7_API UFP_SkillLeafBase : public UFP_Leaf
{
	GENERATED_BODY()

public:
	virtual void PopulateSkillData(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS) {}
};
