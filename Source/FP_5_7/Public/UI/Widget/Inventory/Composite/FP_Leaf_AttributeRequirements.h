// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "FP_Leaf.h"
#include "FP_Leaf_AttributeRequirements.generated.h"

class URichTextBlock;

/**
 * Leaf widget displaying a single rich-text line of attribute requirements.
 * e.g. "REQUIRES 5 MIGHT, 6 FORTITUDE" — each stat colored red when the
 * player falls short, white when met. Stats set to 0 are omitted entirely.
 * If all requirements are 0 the leaf is not expanded.
 * Populated by FFP_AttributeRequirementFragment::Assimilate.
 *
 * Setup: assign a UDataTable (row struct FRichTextStyleRow) to the
 * RichTextBlock's Text Style Set containing at minimum rows "normal" and "fail".
 */
UCLASS()
class FP_5_7_API UFP_Leaf_AttributeRequirements : public UFP_Leaf
{
	GENERATED_BODY()

public:

	struct FStatRequirement
	{
		int32 Required{ 0 };
		int32 Current{ 0 };
	};

	void SetRequirements(
		FStatRequirement Level,
		FStatRequirement Might,
		FStatRequirement Resonance,
		FStatRequirement Agility,
		FStatRequirement Fortitude);

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> Text_Requirements;
};
