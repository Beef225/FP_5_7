// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Inventory/Composite/FP_SkillLeafBase.h"
#include "FP_SkillLeaf_ModifierTags.generated.h"

class URichTextBlock;

/**
 * Displays the skill's modifier tags as a comma-separated rich text string.
 * Tags are in the form SkillTagMod.X — only the X leaf portion is shown.
 * e.g. { SkillTagMod.Energy, SkillTagMod.Pistol } → "Energy, Pistol"
 * Collapses when SkillModifierTags is empty.
 */
UCLASS()
class FP_5_7_API UFP_SkillLeaf_ModifierTags : public UFP_SkillLeafBase
{
	GENERATED_BODY()

public:
	virtual void PopulateSkillData(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS) override;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URichTextBlock> RichText_ModifierTags;
};
