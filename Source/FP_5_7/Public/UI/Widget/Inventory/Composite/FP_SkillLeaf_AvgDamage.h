// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Inventory/Composite/FP_SkillLeafBase.h"
#include "FP_SkillLeaf_AvgDamage.generated.h"

class UTextBlock;

/** Average damage per activation against the display dummy (35% resistance, 1000 Armour). Collapses for non-damaging skills. */
UCLASS()
class FP_5_7_API UFP_SkillLeaf_AvgDamage : public UFP_SkillLeafBase
{
	GENERATED_BODY()

public:
	virtual void PopulateSkillData(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS) override;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_AvgDamage;
};
