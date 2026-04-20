// Copyright JG

#include "Inventory/Loot/Data/FP_RarityTable.h"

EFP_ItemRarity UFP_RarityTable::Roll(float TotalIIR) const
{
	if (Entries.IsEmpty()) return EFP_ItemRarity::Common;

	// Diminishing returns: caps the effective boost at 1.0 (100%) as TotalIIR → ∞.
	// Common weight is never multiplied so it always retains its absolute share.
	const float EffectiveBoost = TotalIIR / (1.f + FMath::Max(TotalIIR, 0.f));

	float Total = 0.f;
	for (const FFP_RarityWeight& Entry : Entries)
	{
		float W = Entry.Weight;
		if (Entry.Rarity != EFP_ItemRarity::Common)
			W *= (1.f + EffectiveBoost);
		Total += W;
	}

	if (Total <= 0.f) return EFP_ItemRarity::Common;

	const float RandVal = FMath::FRandRange(0.f, Total);
	float Cumulative = 0.f;
	for (const FFP_RarityWeight& Entry : Entries)
	{
		float W = Entry.Weight;
		if (Entry.Rarity != EFP_ItemRarity::Common)
			W *= (1.f + EffectiveBoost);
		Cumulative += W;
		if (RandVal <= Cumulative)
			return Entry.Rarity;
	}

	return Entries.Last().Rarity;
}

const FFP_RarityWeight* UFP_RarityTable::GetEntry(EFP_ItemRarity Rarity) const
{
	return Entries.FindByPredicate([Rarity](const FFP_RarityWeight& E) { return E.Rarity == Rarity; });
}
