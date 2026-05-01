// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Inventory/Composite/FP_Composite.h"
#include "AbilitySystem/Data/FP_SkillLibrary.h"
#include "FP_SkillDescription.generated.h"

class AFP_PlayerState;

/**
 * Composite root for skill description widgets.
 *
 * Place this as the root (or a named child) in a skill tooltip BP.
 * The widget tree should contain UFP_SkillLeafBase children that each display
 * one aspect of the skill (name, icon, level, XP bar, stats, etc.).
 *
 * Call Populate() to drive all child leaves with data from the skill library entry
 * and the player's current runtime state.
 */
UCLASS()
class FP_5_7_API UFP_SkillDescription : public UFP_Composite
{
	GENERATED_BODY()

public:
	void Populate(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS);
};
