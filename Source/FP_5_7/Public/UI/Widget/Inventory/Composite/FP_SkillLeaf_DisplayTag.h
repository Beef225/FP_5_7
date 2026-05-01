// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Inventory/Composite/FP_SkillLeafBase.h"
#include "FP_SkillLeaf_DisplayTag.generated.h"

class URichTextBlock;

/** Displays the short category label from FFP_AbilityEntry::DisplayTag (e.g. "Ranged, Pistol"). */
UCLASS()
class FP_5_7_API UFP_SkillLeaf_DisplayTag : public UFP_SkillLeafBase
{
	GENERATED_BODY()

public:
	virtual void PopulateSkillData(const FFP_AbilityEntry& Entry, const AFP_PlayerState* PS) override;

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URichTextBlock> RichText_DisplayTag;
};
