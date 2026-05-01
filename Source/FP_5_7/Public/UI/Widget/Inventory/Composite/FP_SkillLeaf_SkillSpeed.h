// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Inventory/Composite/FP_SkillLeafBase.h"
#include "FP_SkillLeaf_SkillSpeed.generated.h"

class UTextBlock;

/** Effective skill speed: BaseSkillSpeed × player SkillSpeed attribute. Collapses if BaseSkillSpeed == 0. */
UCLASS()
class FP_5_7_API UFP_SkillLeaf_SkillSpeed : public UFP_SkillLeafBase
{
	GENERATED_BODY()

public:
	virtual void PopulateSkillData(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS) override;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_SkillSpeed;
};
