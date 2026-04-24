// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Items/Fragments/FP_ItemFragment.h"
#include "Inventory/Items/Affixes/FP_AffixTypes.h"
#include "FP_AffixFragment.generated.h"

class UFP_AffixPool;
class UFP_AffixRegistry;

/**
 * Fragment that drives affix rolling for equippable items.
 *
 * Set AffixPool, Registry, ItemLevel, MaxPrefixes, and MaxSuffixes on the item's
 * manifest before the actor spawns. OnSpawned() performs the full two-stage roll:
 *   Stage 1 — weighted pool pick (respects GroupTag exclusivity and AffinityTag lock)
 *   Stage 2 — weighted tier pick filtered by RequiredItemLevel <= ItemLevel
 *   Stage 3 — uniform roll within tier [Min, Max] at Step resolution
 *
 * Rolled affixes are stored in RolledAffixes and persist with the item.
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_AffixFragment : public FFP_InventoryItemFragment
{
	GENERATED_BODY()

	const TArray<FFP_AffixInstance>& GetRolledAffixes() const { return RolledAffixes; }

	virtual void OnSpawned(FFP_ItemManifest& Manifest) override;
	virtual void Assimilate(UFP_CompositeBase* Composite) const override;

	UPROPERTY(EditAnywhere, Category="Affixes")
	TObjectPtr<UFP_AffixPool> AffixPool;

	UPROPERTY(EditAnywhere, Category="Affixes")
	TObjectPtr<UFP_AffixRegistry> Registry;

	UPROPERTY(EditAnywhere, Category="Affixes", meta=(ClampMin=1))
	int32 ItemLevel{ 1 };

	UPROPERTY(EditAnywhere, Category="Affixes", meta=(ClampMin=0, ClampMax=3))
	int32 MaxPrefixes{ 3 };

	UPROPERTY(EditAnywhere, Category="Affixes", meta=(ClampMin=0, ClampMax=3))
	int32 MaxSuffixes{ 3 };

private:

	UPROPERTY(VisibleAnywhere, Category="Affixes")
	TArray<FFP_AffixInstance> RolledAffixes;

	bool bRolled{ false };

	void RollAffixes();
};
