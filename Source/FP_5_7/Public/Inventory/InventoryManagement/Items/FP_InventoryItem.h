// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UObject/Object.h"
#include "Inventory/Items/Manifest/FP_ItemManifest.h"
#include "StructUtils/InstancedStruct.h"
#include "FP_InventoryItem.generated.h"

/**
 * Base class for all inventory items. Lives as a UObject inside the inventory's
 * FastArray; replicated to clients via FFP_InventoryFastArray.
 */
UCLASS(BlueprintType, Blueprintable)
class FP_5_7_API UFP_InventoryItem : public UObject
{
	GENERATED_BODY()

public:

	virtual bool IsSupportedForNetworking() const override { return true; }

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetItemManifest(const FFP_ItemManifest& Manifest);

	const FFP_ItemManifest& GetItemManifest() const { return ItemManifest.Get<FFP_ItemManifest>(); }
	FFP_ItemManifest& GetItemManifestMutable() { return ItemManifest.GetMutable<FFP_ItemManifest>(); }
	bool IsStackable() const;
	bool IsConsumable() const;

	int32 GetTotalStackCount() const { return TotalStackCount; }
	void SetTotalStackCount(int32 Count) { TotalStackCount = Count; }

private:

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Inventory")
	int32 TotalStackCount{ 0 };

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Inventory",
		meta = (BaseStruct = "/Script/FP_5_7.FP_ItemManifest"))
	FInstancedStruct ItemManifest;
};

/**
 * Convenience free function: retrieves the first fragment of type FragmentType
 * from the item's manifest whose tag matches the given FragmentTag.
 * Returns nullptr if the item is invalid or no matching fragment exists.
 */
template<typename FragmentType>
const FragmentType* GetFragment(const UFP_InventoryItem* Item, const FGameplayTag& Tag)
{
	if (!IsValid(Item)) return nullptr;
	return Item->GetItemManifest().GetFragmentOfTypeWithTag<FragmentType>(Tag);
}
