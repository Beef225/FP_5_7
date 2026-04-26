// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "FP_Leaf.h"
#include "Inventory/Items/Affixes/FP_AffixTypes.h"
#include "FP_Leaf_Implicits.generated.h"

class URichTextBlock;

/**
 * Leaf widget that displays all rolled implicit modifiers for an item.
 * Follows the same rich-text pattern as UFP_Leaf_Affixes.
 * Populated by FFP_ImplicitFragment::Assimilate when an item is hovered.
 */
UCLASS()
class FP_5_7_API UFP_Leaf_Implicits : public UFP_Leaf
{
	GENERATED_BODY()

public:

	void SetImplicits(const TArray<FFP_AffixInstance>& Implicits);

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> RichText_Implicits;
};
