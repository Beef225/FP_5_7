// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Inventory/Composite/FP_Composite.h"
#include "FP_SkillTreeNodeDescription.generated.h"

class UFP_SkillTreeNodeData;

/**
 * Composite root for a skill tree node description tooltip.
 * Contains UFP_SkillTreeLeaf_Header and UFP_SkillTreeLeaf_NodeDesc children.
 */
UCLASS()
class FP_5_7_API UFP_SkillTreeNodeDescription : public UFP_Composite
{
	GENERATED_BODY()

public:

	void Populate(const UFP_SkillTreeNodeData* Data);
};
