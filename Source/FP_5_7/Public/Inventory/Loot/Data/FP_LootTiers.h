// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FP_LootTiers.generated.h"


USTRUCT(BlueprintType)
struct FLootItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LootTiers|Spawning")
	TSubclassOf<AActor> LootClass;

	/**
	 * Chance to spawn. Each whole number is a guaranteed drop; the fractional part is the
	 * probability of one additional drop. E.g. 2.5 = 2 guaranteed + 50% chance of a third.
	 */
	UPROPERTY(EditAnywhere, Category = "LootTiers|Spawning", meta = (ClampMin = "0.0"))
	float ChanceToSpawn = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LootTiers|Spawning")
	bool bLootLevelOverride = true;
};

/** One entry inside a weighted pool — assign a relative weight (does not need to sum to 100). */
USTRUCT(BlueprintType)
struct FWeightedLootItem
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LootTiers|Pool")
	TSubclassOf<AActor> LootClass;

	/**
	 * Relative weight for this item within the pool.
	 * Higher weight = more likely to be chosen.
	 * E.g. weights of 50, 10, 1, 39 give 50%, 10%, 1%, 39% chances respectively.
	 */
	UPROPERTY(EditAnywhere, Category = "LootTiers|Pool", meta = (ClampMin = "0.0"))
	float Weight = 1.f;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LootTiers|Pool")
	bool bLootLevelOverride = true;
};

/**
 * A pool that guarantees exactly one drop chosen by weighted random.
 * Add multiple pools to guarantee one drop from each independent category
 * (e.g. one weapon pool + one accessory pool both fire on the same enemy death).
 */
USTRUCT(BlueprintType)
struct FLootPool
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "LootTiers|Pool")
	TArray<FWeightedLootItem> Items;
};

/**
 * Data asset that defines what an enemy (or group of enemies) can drop.
 *
 * LootItems      — independent rolls, each item checked separately (ChanceToSpawn logic).
 * GuaranteedPools — each pool guarantees exactly one item chosen by weighted random.
 */
UCLASS()
class FP_5_7_API UFP_LootTiers : public UDataAsset
{
	GENERATED_BODY()

public:

	/** Returns all items that pass their independent spawn rolls, plus one winner per guaranteed pool. */
	UFUNCTION(BlueprintCallable)
	TArray<FLootItem> GetLootItems();

	UPROPERTY(EditDefaultsOnly, Category = "LootTiers|Spawning")
	TArray<FLootItem> LootItems;

	/** Each pool picks exactly one item by weight — guaranteed to produce one drop per pool. */
	UPROPERTY(EditDefaultsOnly, Category = "LootTiers|GuaranteedPools")
	TArray<FLootPool> GuaranteedPools;
};
