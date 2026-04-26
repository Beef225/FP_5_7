// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FP_RareNameTable.generated.h"

/**
 * One rare name pool for a specific item type (e.g. Pistol, Sword, Headwear).
 * Holds separate prefix and suffix word lists. The importer creates one asset
 * per sheet in itemnamedatabase.xlsx.
 */
UCLASS(BlueprintType)
class FP_5_7_API UFP_RareNameTable : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	/** The item type this table covers, matching the sheet name (e.g. "Pistol"). */
	UPROPERTY(VisibleAnywhere, Category="RareName")
	FName ItemType;

	UPROPERTY(VisibleAnywhere, Category="RareName")
	TArray<FName> Prefixes;

	UPROPERTY(VisibleAnywhere, Category="RareName")
	TArray<FName> Suffixes;

	FName PickPrefix() const { return Prefixes.IsEmpty() ? NAME_None : Prefixes[FMath::RandRange(0, Prefixes.Num() - 1)]; }
	FName PickSuffix() const { return Suffixes.IsEmpty() ? NAME_None : Suffixes[FMath::RandRange(0, Suffixes.Num() - 1)]; }
};
