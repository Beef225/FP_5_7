// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Inventory/Items/Fragments/FP_ItemFragment.h"
#include "Libraries/FP_EnumDefs.h"
#include "StructUtils/InstancedStruct.h"
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

	/**
	 * Returns a const pointer to the first fragment of type T whose FragmentTag matches,
	 * or nullptr if none is found. Requires T to derive from FFP_ItemFragment.
	 */
	template<typename T> requires std::derived_from<T, FFP_ItemFragment>
	const T* GetFragmentOfTypeWithTag(const FGameplayTag& FragmentTag) const;

private:

	/**
	 * Composable data fragments for this item (e.g. grid size, display info, stats).
	 * ExcludeBaseStruct prevents the raw FFP_ItemFragment base from appearing in the editor picker —
	 * only concrete subclasses such as FFP_GridFragment are selectable.
	 */
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FFP_ItemFragment>> Fragments;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	EItemCategory ItemCategory{ EItemCategory::None };

	/** Specific item type tag, e.g. Inventory.Equippable.Weapon. Used for filtering, sorting, and rules. */
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (Categories = "Inventory"))
	FGameplayTag ItemType;
};

template<typename T>
requires std::derived_from<T, FFP_ItemFragment>
const T* FFP_ItemManifest::GetFragmentOfTypeWithTag(const FGameplayTag& FragmentTag) const
{
	for (const TInstancedStruct<FFP_ItemFragment>& Fragment : Fragments)
	{
		if (const T* FragmentPtr = Fragment.GetPtr<T>())
		{
			if (!FragmentPtr->GetFragmentTag().MatchesTagExact(FragmentTag)) continue;
			return FragmentPtr;
		}
	}
	return nullptr;
}
