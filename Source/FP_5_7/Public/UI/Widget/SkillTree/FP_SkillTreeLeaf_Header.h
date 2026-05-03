// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Inventory/Composite/FP_Leaf.h"
#include "FP_SkillTreeLeaf_Header.generated.h"

class UTextBlock;
class UFP_SkillTreeNodeData;

/**
 * Leaf widget displaying the node name and (optionally) the node type label.
 */
UCLASS()
class FP_5_7_API UFP_SkillTreeLeaf_Header : public UFP_Leaf
{
	GENERATED_BODY()

public:

	void SetNodeData(const UFP_SkillTreeNodeData* Data);

protected:

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_NodeName;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_NodeType;
};
