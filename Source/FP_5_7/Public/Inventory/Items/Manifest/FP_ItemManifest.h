// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Inventory/Items/Fragments/FP_ItemFragment.h"
#include "Libraries/FP_EnumDefs.h"
#include "StructUtils/InstancedStruct.h"
#include "FP_ItemManifest.generated.h"

class UFP_InventoryItem;
class UFP_CompositeBase;

/**
 * Holds all the data required to create a new UFP_InventoryItem.
 * Placed on world pickup actors via UFP_ItemComponent.
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_ItemManifest
{
	GENERATED_BODY()

	TArray<TInstancedStruct<FFP_ItemFragment>>& GetFragmentsMutable() { return Fragments; }

	/** Factory: spawns and returns a fully initialised UFP_InventoryItem. */
	UFP_InventoryItem* Manifest(UObject* NewOuter);

	/** Spawns the pickup actor for this manifest at the given location/rotation. */
	void SpawnPickupActor(const UObject* WorldContextObject, const FVector& SpawnLocation, const FRotator& SpawnRotation);

	EItemCategory GetItemCategory() const { return ItemCategory; }
	FGameplayTag GetItemType() const { return ItemType; }

	/**
	 * Iterates all FFP_InventoryItemFragment-derived fragments and calls Assimilate on each
	 * leaf of the composite widget tree, expanding leaves whose tags match.
	 */
	void AssimilateInventoryFragments(UFP_CompositeBase* Composite) const;

	/**
	 * Calls OnSpawned() on every fragment. Invoked from AFP_ItemActor::BeginPlay so
	 * fragments can roll random values or do one-time setup at world-spawn time.
	 */
	void NotifyItemSpawned();

	/**
	 * Returns all fragments of type T (or any type derived from T).
	 */
	template<typename T> requires std::derived_from<T, FFP_ItemFragment>
	TArray<const T*> GetAllFragmentsOfType() const;

	/**
	 * Returns a const pointer to the first fragment of type T whose FragmentTag matches,
	 * or nullptr if none is found. Requires T to derive from FFP_ItemFragment.
	 */
	template<typename T> requires std::derived_from<T, FFP_ItemFragment>
	const T* GetFragmentOfTypeWithTag(const FGameplayTag& FragmentTag) const;

	/** Returns a const pointer to the first fragment of type T, regardless of tag. */
	template<typename T> requires std::derived_from<T, FFP_ItemFragment>
	const T* GetFragmentOfType() const;

	/** Returns a mutable pointer to the first fragment of type T — use to write back to live fragments. */
	template<typename T> requires std::derived_from<T, FFP_ItemFragment>
	T* GetFragmentOfTypeMutable();

private:

	/**
	 * Composable data fragments for this item (e.g. grid size, display info, stats).
	 * ExcludeBaseStruct prevents the raw FFP_ItemFragment base from appearing in the editor picker —
	 * only concrete subclasses such as FFP_GridFragment are selectable.
	 */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<AActor> PickupActorClass;

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FFP_ItemFragment>> Fragments;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	EItemCategory ItemCategory{ EItemCategory::None };

	/** Specific item type tag, e.g. Inventory.Equippable.Weapon. Used for filtering, sorting, and rules. */
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (Categories = "Inventory"))
	FGameplayTag ItemType;

	/** Resets and empties the Fragments array — called after Manifest() copies fragments to the inventory item. */
	void ClearFragments();
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

template<typename T>
requires std::derived_from<T, FFP_ItemFragment>
const T* FFP_ItemManifest::GetFragmentOfType() const
{
	for (const TInstancedStruct<FFP_ItemFragment>& Fragment : Fragments)
	{
		if (const T* FragmentPtr = Fragment.GetPtr<T>())
		{
			return FragmentPtr;
		}
	}
	return nullptr;
}

template<typename T>
requires std::derived_from<T, FFP_ItemFragment>
T* FFP_ItemManifest::GetFragmentOfTypeMutable()
{
	for (TInstancedStruct<FFP_ItemFragment>& Fragment : Fragments)
	{
		if (T* FragmentPtr = Fragment.GetMutablePtr<T>())
		{
			return FragmentPtr;
		}
	}
	return nullptr;
}

template<typename T>
requires std::derived_from<T, FFP_ItemFragment>
TArray<const T*> FFP_ItemManifest::GetAllFragmentsOfType() const
{
	TArray<const T*> Result;
	for (const TInstancedStruct<FFP_ItemFragment>& Fragment : Fragments)
	{
		if (const T* FragmentPtr = Fragment.GetPtr<T>())
		{
			Result.Add(FragmentPtr);
		}
	}
	return Result;
}
