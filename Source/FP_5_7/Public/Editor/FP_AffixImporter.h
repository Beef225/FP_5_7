// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FP_AffixImporter.generated.h"

/**
 * Editor utility that reads affixes.json and pools.json from
 * Tools/MasterDatabase/Affixes/ and creates (or updates) the corresponding
 * UFP_AffixDefinition, UFP_AffixPool, and UFP_AffixRegistry data assets.
 *
 * Usage: create a Data Asset of this type in the content browser, set
 * OutputContentPath, open it, and click the "Import Affixes" button.
 */
UCLASS(BlueprintType)
class FP_5_7_API UFP_AffixImporter : public UDataAsset
{
	GENERATED_BODY()

public:

	/** Content browser path where all affix assets are written. */
	UPROPERTY(EditAnywhere, Category="Import")
	FString OutputContentPath{ TEXT("/Game/Data/Affixes") };

	/**
	 * Reads affixes.json and pools.json and creates or updates all
	 * UFP_AffixDefinition, UFP_AffixPool, and UFP_AffixRegistry assets.
	 */
	UFUNCTION(CallInEditor, Category="Import")
	void ImportAffixes();
};
