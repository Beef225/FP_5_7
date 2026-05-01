// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Inventory/Composite/FP_SkillLeafBase.h"
#include "FP_SkillLeaf_DPS.generated.h"

class UTextBlock;

/** Average damage × effective skill speed. Collapses for non-damaging or zero-speed skills. */
UCLASS()
class FP_5_7_API UFP_SkillLeaf_DPS : public UFP_SkillLeafBase
{
	GENERATED_BODY()

public:
	virtual void PopulateSkillData(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS) override;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_DPS;
};
