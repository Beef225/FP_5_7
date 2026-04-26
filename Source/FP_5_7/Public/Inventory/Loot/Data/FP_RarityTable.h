// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Libraries/FP_EnumDefs.h"
#include "FP_RarityTable.generated.h"

/**
 * One entry in a rarity table. The Weight is an arbitrary positive number —
 * only the ratios between entries matter. Common's weight is never boosted by IIR
 * so it always retains its absolute share of the total, providing a natural floor.
 *
 * MinPrefixes/MaxPrefixes/MinSuffixes/MaxSuffixes define the affix budget for this
 * rarity tier. Unused until the affix system is implemented.
 */
USTRUCT(BlueprintType)
struct FFP_RarityWeight
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rarity")
	EFP_ItemRarity Rarity{ EFP_ItemRarity::Common };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Rarity", meta=(ClampMin="0.0"))
	float Weight{ 100.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	int32 MinPrefixes{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	int32 MaxPrefixes{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	int32 MinSuffixes{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	int32 MaxSuffixes{ 0 };
};

/**
 * Data asset that defines the weighted rarity distribution for a category of items.
 *
 * Multiple IIR sources (character attribute, monster difficulty, area level) should be
 * summed by the caller and passed as TotalIIR. Diminishing returns are applied internally:
 *
 *   EffectiveBoost = TotalIIR / (1 + TotalIIR)
 *
 * This caps the boost at 1.0 (100%) regardless of how much IIR is stacked, so Common
 * always keeps its absolute weight and can never be fully crowded out.
 *
 * Example: Common=500, Uncommon=200, Rare=50, Legendary=5
 *   TotalIIR=0   → Common ~70%, Uncommon ~26%, Rare ~6.5%, Legendary ~0.65%
 *   TotalIIR=1   → EffectiveBoost=0.5 → non-common weights ×1.5
 *   TotalIIR=∞   → EffectiveBoost→1.0 → non-common weights ×2.0 (hard cap)
 */
UCLASS(BlueprintType)
class FP_5_7_API UFP_RarityTable : public UDataAsset
{
	GENERATED_BODY()

public:

	/**
	 * Rolls a rarity from the table.
	 * @param TotalIIR  Sum of all stacked Increased Item Rarity modifiers (0.05 = 5%).
	 */
	EFP_ItemRarity Roll(float TotalIIR = 0.f) const;

	/** Returns the entry for a specific rarity, or nullptr if not in this table. */
	const FFP_RarityWeight* GetEntry(EFP_ItemRarity Rarity) const;

private:

	UPROPERTY(EditAnywhere, Category="Rarity")
	TArray<FFP_RarityWeight> Entries;
};
