// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Libraries/FP_EnumDefs.h"
#include "FP_ItemManifest.generated.h"

class UFP_InventoryItem;

/**
 * Holds all the data required to create a new UFP_InventoryItem.
 * Placed on world pickup actors via UFP_ItemComponent.
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_ItemManifest
{
	GENERATED_BODY()

	/** Factory: spawns and returns a fully initialised UFP_InventoryItem. */
	UFP_InventoryItem* Manifest(UObject* NewOuter);

	EItemCategory GetItemCategory() const { return ItemCategory; }

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	EItemCategory ItemCategory{ EItemCategory::None };
};
