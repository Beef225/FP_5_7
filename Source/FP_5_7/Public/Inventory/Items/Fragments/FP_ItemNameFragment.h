// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Items/Fragments/FP_ItemFragment.h"
#include "Libraries/FP_EnumDefs.h"
#include "Inventory/Items/ItemNames/FP_RareNameTable.h"
#include "FP_ItemNameFragment.generated.h"

/**
 * Generates the item's display name based on rarity at spawn time (third pass, after affixes roll).
 *
 * Common    : TypeName only
 * Uncommon  : "[PrefixTierName] [TypeName] [SuffixTierName]"  (uses first prefix/suffix rolled)
 * Rare+     : TypeName fallback for now; dedicated systems added later
 *
 * Assimilate():
 *   Fragment.ItemName leaf  → expanded and populated with GeneratedName
 *   Fragment.ItemType leaf  → collapsed for Common/Uncommon (type is already in the name);
 *                             expanded for Rare and above
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_ItemNameFragment : public FFP_InventoryItemFragment
{
	GENERATED_BODY()

	FText GetGeneratedName() const { return GeneratedName; }
	EFP_ItemRarity GetCachedRarity() const { return CachedRarity; }

	virtual void OnSpawned(FFP_ItemManifest& Manifest) override;
	virtual void Assimilate(UFP_CompositeBase* Composite) const override;

	/** Name pool for Rare items. Set this on each item's manifest to the matching table asset. */
	UPROPERTY(EditAnywhere, Category="ItemName")
	TObjectPtr<UFP_RareNameTable> RareNameTable;

private:

	UPROPERTY(VisibleAnywhere, Category="ItemName")
	FText GeneratedName;

	UPROPERTY(VisibleAnywhere, Category="ItemName")
	EFP_ItemRarity CachedRarity{ EFP_ItemRarity::Common };
};
