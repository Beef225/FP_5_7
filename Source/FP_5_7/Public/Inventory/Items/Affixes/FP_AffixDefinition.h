// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Libraries/FP_EnumDefs.h"
#include "Inventory/Items/Affixes/FP_AffixTypes.h"
#include "FP_AffixDefinition.generated.h"

/**
 * Data asset for one affix entry from affixes.json.
 * Holds the affix metadata plus all tier rows.
 * Created by the editor utility that reads affixes.json.
 */
UCLASS(BlueprintType)
class FP_5_7_API UFP_AffixDefinition : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	FName AffixID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	EFP_AffixType Type{ EFP_AffixType::Prefix };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	FName GroupTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	FName DisplayTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	FGameplayTag Stat1_Attr;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	FGameplayTag Stat2_Attr;

	/** Format string for tooltip display, e.g. "+{1} Physical Damage". */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	FString DisplayFormat;

	/** All tiers, sorted descending by Tier number (T8 weakest first, T1 strongest last). */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	TArray<FFP_AffixTierData> Tiers;

	/**
	 * Weighted-random tier selection filtered by item level.
	 * Only tiers with RequiredItemLevel <= ItemLevel are eligible.
	 * Returns nullptr if no tier qualifies.
	 */
	const FFP_AffixTierData* PickTier(int32 ItemLevel) const;
};

/**
 * Data asset for one pool sheet from pools.json.
 * Holds an ordered list of pool entries (GroupTag / AffixID / Weight / AffinityTag).
 * Created by the editor utility that reads pools.json.
 */
UCLASS(BlueprintType)
class FP_5_7_API UFP_AffixPool : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	TArray<FFP_AffixPoolEntry> Entries;
};

/**
 * Central registry mapping AffixID -> UFP_AffixDefinition.
 * One registry asset lives in the project; it is referenced by every FFP_AffixFragment.
 * The editor utility populates this map when it imports affixes.json.
 */
UCLASS(BlueprintType)
class FP_5_7_API UFP_AffixRegistry : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	TMap<FName, TObjectPtr<UFP_AffixDefinition>> Definitions;

	UFUNCTION(BlueprintCallable, Category="Affixes")
	const UFP_AffixDefinition* Find(FName AffixID) const;
};
