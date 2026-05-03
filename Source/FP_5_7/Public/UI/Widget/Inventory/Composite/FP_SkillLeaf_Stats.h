// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Inventory/Composite/FP_SkillLeafBase.h"
#include "FP_SkillLeaf_Stats.generated.h"

class UTextBlock;

/**
 * Displays the three core skill stats: Damage/Sec, Cast Speed, and Radius.
 * Individual stat rows collapse when their value is zero / default (Speed row
 * is hidden if BaseSkillSpeed == 1.0 and Radius row if BaseRadius == 0).
 * The whole leaf collapses when DamagePerSecond == 0 and BaseRadius == 0.
 */
UCLASS()
class FP_5_7_API UFP_SkillLeaf_Stats : public UFP_SkillLeafBase
{
	GENERATED_BODY()

public:
	virtual void PopulateSkillData(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS) override;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_DPS_Value;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Speed_Value;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Radius_Value;
};
