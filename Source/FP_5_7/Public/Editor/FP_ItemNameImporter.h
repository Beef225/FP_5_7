// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FP_ItemNameImporter.generated.h"

/**
 * Editor utility that reads itemnames.json from Tools/MasterDatabase/ItemNames/
 * and creates (or updates) one UFP_RareNameTable data asset per item type.
 *
 * Usage: create a Data Asset of this type in the content browser, set
 * OutputContentPath, open it, and click the "Import Item Names" button.
 */
UCLASS(BlueprintType)
class FP_5_7_API UFP_ItemNameImporter : public UDataAsset
{
	GENERATED_BODY()

public:

	/** Content browser path where all name table assets are written. */
	UPROPERTY(EditAnywhere, Category="Import")
	FString OutputContentPath{ TEXT("/Game/Data/ItemNames") };

	/** Reads itemnames.json and creates or updates all UFP_RareNameTable assets. */
	UFUNCTION(CallInEditor, Category="Import")
	void ImportItemNames();
};
