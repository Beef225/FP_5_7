// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Items/Fragments/FP_ItemFragment.h"
#include "Inventory/Items/Affixes/FP_AffixTypes.h"
#include "GameplayEffectTypes.h"
#include "FP_ImplicitFragment.generated.h"

class UFP_AffixDefinition;
class APlayerController;

/**
 * One implicit slot on an item. Points at a specific affix definition and which tier
 * of that affix to use. The stat value is rolled randomly within the tier's range at
 * spawn time — ignoring the tier's RequiredItemLevel, so common items can have implicits.
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_ImplicitEntry
{
	GENERATED_BODY()

	/** The affix whose tier range defines the roll. */
	UPROPERTY(EditAnywhere, Category="Implicit")
	TObjectPtr<UFP_AffixDefinition> AffixDef;

	/** Tier number to use (matches the Tier column in the Excel sheet, e.g. 1 = highest). */
	UPROPERTY(EditAnywhere, Category="Implicit", meta=(ClampMin=1))
	int32 Tier{ 1 };
};

/**
 * Implicit modifier fragment. Holds one or more fixed affix entries that roll at spawn
 * regardless of item rarity or item level. Common items can carry implicits; the tier is
 * set by the designer, not the loot system.
 *
 * OnEquip / OnUnequip and the display leaf are added separately.
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_ImplicitFragment : public FFP_InventoryItemFragment
{
	GENERATED_BODY()

	const TArray<FFP_AffixInstance>& GetRolledImplicits() const { return RolledImplicits; }

	virtual void OnSpawned(FFP_ItemManifest& Manifest) override;
	virtual void Assimilate(UFP_CompositeBase* Composite) const override;

	void OnEquip(APlayerController* PC);
	void OnUnequip(APlayerController* PC);

	UPROPERTY(EditAnywhere, Category="Implicit")
	TArray<FFP_ImplicitEntry> Entries;

private:

	UPROPERTY(VisibleAnywhere, Category="Implicit")
	TArray<FFP_AffixInstance> RolledImplicits;

	bool bRolled{ false };

	FActiveGameplayEffectHandle ActiveEffectHandle;
};
