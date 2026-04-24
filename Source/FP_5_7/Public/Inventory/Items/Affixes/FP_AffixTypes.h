// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Libraries/FP_EnumDefs.h"
#include "FP_AffixTypes.generated.h"

/**
 * One tier row from the affix master list.
 * Tiers are sorted descending by Tier number (T8 weakest first, T1 strongest last)
 * so the array read order matches the data asset ordering.
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_AffixTierData
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	int32 Tier{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	FName ItemName;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	int32 RequiredItemLevel{ 1 };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	int32 Weight{ 100 };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	float Stat1_Min{ 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	float Stat1_Max{ 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	float Stat1_Step{ 1.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	int32 Stat1_DecimalCount{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	EFP_AffixDisplayAs Stat1_DisplayAs{ EFP_AffixDisplayAs::Flat };

	// Stat2 fields — Stat2_Step == 0 means this tier has no second stat.
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	float Stat2_Min{ 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	float Stat2_Max{ 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	float Stat2_Step{ 0.f };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	int32 Stat2_DecimalCount{ 0 };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	EFP_AffixDisplayAs Stat2_DisplayAs{ EFP_AffixDisplayAs::Flat };

	bool HasStat2() const { return Stat2_Step > 0.f; }
};

/**
 * One entry in an affix pool sheet (GroupTag / AffixID / PoolWeight / AffinityTag).
 * AffinityTag empty means affinity-neutral — never filtered by affinity lock.
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_AffixPoolEntry
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	FName GroupTag;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	FName AffixID;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	int32 PoolWeight{ 100 };

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Affixes")
	FName AffinityTag;
};

/**
 * A fully rolled affix instance stored on an item.
 * Created by FFP_AffixFragment::OnSpawned() and serialized with the item.
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_AffixInstance
{
	GENERATED_BODY()

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Affixes")
	FName AffixID;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Affixes")
	EFP_AffixType AffixType{ EFP_AffixType::Prefix };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Affixes")
	FName GroupTag;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Affixes")
	int32 Tier{ 0 };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Affixes")
	FName TierName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Affixes")
	FGameplayTag Stat1_Attr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Affixes")
	float Stat1_Value{ 0.f };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Affixes")
	int32 Stat1_DecimalCount{ 0 };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Affixes")
	EFP_AffixDisplayAs Stat1_DisplayAs{ EFP_AffixDisplayAs::Flat };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Affixes")
	FGameplayTag Stat2_Attr;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Affixes")
	float Stat2_Value{ 0.f };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Affixes")
	int32 Stat2_DecimalCount{ 0 };

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Affixes")
	EFP_AffixDisplayAs Stat2_DisplayAs{ EFP_AffixDisplayAs::Flat };

	/** Format string from the master list, e.g. "+{1} Physical Damage". */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Affixes")
	FString DisplayFormat;

	bool HasStat2() const { return Stat2_Attr.IsValid(); }
};
