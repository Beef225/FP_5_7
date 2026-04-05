// Copyright JG


#include "Inventory/Loot/Data/FP_LootTiers.h"

TArray<FLootItem> UFP_LootTiers::GetLootItems()
{
	TArray<FLootItem> ReturnItems;

	// --- Independent rolls ---
	for (const FLootItem& Item : LootItems)
	{
		if (!Item.LootClass) continue;

		const int32 GuaranteedCount = FMath::FloorToInt(Item.ChanceToSpawn);
		const float BonusChance     = Item.ChanceToSpawn - static_cast<float>(GuaranteedCount);
		const int32 SpawnCount      = GuaranteedCount + (FMath::FRand() < BonusChance ? 1 : 0);

		for (int32 i = 0; i < SpawnCount; ++i)
		{
			FLootItem NewItem;
			NewItem.LootClass          = Item.LootClass;
			NewItem.bLootLevelOverride = Item.bLootLevelOverride;
			ReturnItems.Add(NewItem);
		}
	}

	// --- Guaranteed pools: pick exactly one item per pool by weighted random ---
	for (const FLootPool& Pool : GuaranteedPools)
	{
		if (Pool.Items.IsEmpty()) continue;

		// Sum all weights
		float TotalWeight = 0.f;
		for (const FWeightedLootItem& Entry : Pool.Items)
		{
			TotalWeight += FMath::Max(0.f, Entry.Weight);
		}
		if (TotalWeight <= 0.f) continue;

		// Roll and walk down the list until we find the winner
		float Roll = FMath::FRandRange(0.f, TotalWeight);
		for (const FWeightedLootItem& Entry : Pool.Items)
		{
			Roll -= FMath::Max(0.f, Entry.Weight);
			if (Roll <= 0.f)
			{
				if (Entry.LootClass)
				{
					FLootItem NewItem;
					NewItem.LootClass          = Entry.LootClass;
					NewItem.bLootLevelOverride = Entry.bLootLevelOverride;
					ReturnItems.Add(NewItem);
				}
				break;
			}
		}
	}

	return ReturnItems;
}
