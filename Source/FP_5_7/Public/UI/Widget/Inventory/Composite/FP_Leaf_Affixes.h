// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "FP_Leaf.h"
#include "Inventory/Items/Affixes/FP_AffixTypes.h"
#include "FP_Leaf_Affixes.generated.h"

class URichTextBlock;

/**
 * Leaf widget that displays all rolled affixes for an item.
 *
 * Affixes are shown as formatted text lines using each affix's DisplayFormat string
 * (e.g. "+{1} Physical Damage" → "+14 Physical Damage"). Prefixes are always shown
 * above suffixes; within each group, order matches the pool sheet ordering so the same
 * affix types appear in a consistent position across all items of that slot.
 *
 * Uses URichTextBlock so inline rich-text markup can be added per-affix line later
 * (e.g. coloured tier names, bold stat values) without changing the underlying data flow.
 * Styling is configured through the widget Blueprint's RichText style datatable.
 *
 * Populated by FFP_AffixFragment::Assimilate when an item is hovered.
 */
UCLASS()
class FP_5_7_API UFP_Leaf_Affixes : public UFP_Leaf
{
	GENERATED_BODY()

public:

	void SetAffixes(const TArray<FFP_AffixInstance>& SortedAffixes);

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> RichText_Affixes;
};
