// Copyright JG

#include "Inventory/Items/Affixes/FP_AffixDefinition.h"

const FFP_AffixTierData* UFP_AffixDefinition::PickTier(int32 ItemLevel) const
{
	TArray<const FFP_AffixTierData*> Eligible;
	int32 TotalWeight = 0;

	for (const FFP_AffixTierData& Tier : Tiers)
	{
		if (Tier.RequiredItemLevel <= ItemLevel && Tier.Weight > 0)
		{
			Eligible.Add(&Tier);
			TotalWeight += Tier.Weight;
		}
	}

	if (Eligible.IsEmpty() || TotalWeight <= 0) return nullptr;

	int32 Roll = FMath::RandRange(0, TotalWeight - 1);
	int32 Cumulative = 0;
	for (const FFP_AffixTierData* Tier : Eligible)
	{
		Cumulative += Tier->Weight;
		if (Roll < Cumulative) return Tier;
	}
	return Eligible.Last();
}

const UFP_AffixDefinition* UFP_AffixRegistry::Find(FName AffixID) const
{
	const TObjectPtr<UFP_AffixDefinition>* Found = Definitions.Find(AffixID);
	return Found ? Found->Get() : nullptr;
}
