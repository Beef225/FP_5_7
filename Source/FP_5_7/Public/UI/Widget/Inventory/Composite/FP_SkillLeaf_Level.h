// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Inventory/Composite/FP_SkillLeafBase.h"
#include "FP_SkillLeaf_Level.generated.h"

class UTextBlock;

/**
 * Displays current skill level and cap: "LVL 3 / 20" or "LVL MAX" when capped.
 * Collapses for skills with MaxLevel == 1 (non-leveling skills).
 */
UCLASS()
class FP_5_7_API UFP_SkillLeaf_Level : public UFP_SkillLeafBase
{
	GENERATED_BODY()

public:
	virtual void PopulateSkillData(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS) override;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Level;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_MaxLevel;
};
