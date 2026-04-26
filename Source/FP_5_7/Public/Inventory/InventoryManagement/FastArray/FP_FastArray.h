// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Items/FP_ItemComponent.h"
#include "Net/Serialization/FastArraySerializer.h"
#include "FP_FastArray.generated.h"

class UFP_InventoryComponent;
class UFP_InventoryItem;

/** A single entry in the inventory — wraps one UFP_InventoryItem for replication. */
USTRUCT(BlueprintType)
struct FFP_InventoryEntry : public FFastArraySerializerItem
{
	GENERATED_BODY()

	FFP_InventoryEntry() {}

private:
	friend struct FFP_InventoryFastArray;
	friend UFP_InventoryComponent;

	UPROPERTY()
	TObjectPtr<UFP_InventoryItem> Item = nullptr;
};

/** Replicated list of inventory items using delta serialization. */
USTRUCT(BlueprintType)
struct FFP_InventoryFastArray : public FFastArraySerializer
{
	GENERATED_BODY()

	FFP_InventoryFastArray() : OwnerComponent(nullptr) {}
	FFP_InventoryFastArray(UActorComponent* InOwnerComponent) : OwnerComponent(InOwnerComponent) {}

	TArray<UFP_InventoryItem*> GetAllItems() const;
	UFP_InventoryItem* FindFirstItemByType(const FGameplayTag& ItemType);

	// FFastArraySerializer contract
	void PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize);
	void PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize);
	// End FFastArraySerializer contract

	bool NetDeltaSerialize(FNetDeltaSerializeInfo& DeltaParams)
	{
		return FastArrayDeltaSerialize<FFP_InventoryEntry, FFP_InventoryFastArray>(Entries, DeltaParams, *this);
	}

	/** Add from an item component (world pickup). Implemented once UFP_ItemComponent is complete. */
	UFP_InventoryItem* AddEntry(UFP_ItemComponent* ItemComponent);

	/** Add a pre-created item directly (server only). */
	UFP_InventoryItem* AddEntry(UFP_InventoryItem* Item);

	void RemoveEntry(UFP_InventoryItem* Item);

private:
	friend UFP_InventoryComponent;

	UPROPERTY()
	TArray<FFP_InventoryEntry> Entries;

	UPROPERTY(NotReplicated)
	TObjectPtr<UActorComponent> OwnerComponent;
};

template<>
struct TStructOpsTypeTraits<FFP_InventoryFastArray> : public TStructOpsTypeTraitsBase2<FFP_InventoryFastArray>
{
	enum { WithNetDeltaSerializer = true };
};
