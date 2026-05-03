// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Inventory/Composite/FP_Leaf.h"
#include "FP_SkillTreeLeaf_NodeDesc.generated.h"

class UTextBlock;

/**
 * Leaf widget displaying the node's flavour/description text.
 * Collapses itself when the description is empty.
 */
UCLASS()
class FP_5_7_API UFP_SkillTreeLeaf_NodeDesc : public UFP_Leaf
{
	GENERATED_BODY()

public:

	void SetDescription(const FText& Desc);

protected:

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_Description;
};
