// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
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
	FGameplayTag GetItemType() const { return ItemType; }

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	EItemCategory ItemCategory{ EItemCategory::None };

	/** Specific item type tag, e.g. Inventory.Equippable.Weapon. Used for filtering, sorting, and rules. */
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (Categories = "Inventory"))
	FGameplayTag ItemType;
};
