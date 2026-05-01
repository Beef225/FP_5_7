// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Inventory/Composite/FP_SkillLeafBase.h"
#include "FP_SkillLeaf_XPBar.generated.h"

class UProgressBar;
class UTextBlock;

/**
 * XP progress bar showing fill toward the next level.
 * Text_XPCurrent shows XP earned since this level; Text_XPNext shows XP needed for the next level.
 * Collapses for skills with no XP curve or MaxLevel == 1.
 */
UCLASS()
class FP_5_7_API UFP_SkillLeaf_XPBar : public UFP_SkillLeafBase
{
	GENERATED_BODY()

public:
	virtual void PopulateSkillData(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS) override;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_XP;

	/** XP earned toward the current level threshold. */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_XPCurrent;

	/** XP required to reach the next level. */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_XPNext;
};
