// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "FP_InventoryTypes.generated.h"

class UFP_InventoryItem;

/** Availability state for a single grid slot. */
USTRUCT()
struct FFP_SlotAvailability
{
	GENERATED_BODY()

	FFP_SlotAvailability() {}
	FFP_SlotAvailability(int32 ItemIndex, int32 Room, bool bHasItem)
		: Index(ItemIndex), AmountToFill(Room), bItemAtIndex(bHasItem) {}

	int32 Index       { INDEX_NONE };
	int32 AmountToFill{ 0 };
	bool  bItemAtIndex{ false };
};

/** Aggregated result of checking whether the inventory has room for an incoming item. */
USTRUCT()
struct FFP_SlotAvailabilityResult
{
	GENERATED_BODY()

	FFP_SlotAvailabilityResult() {}

	TWeakObjectPtr<UFP_InventoryItem> Item;

	/** Total stack capacity available across all matching slots. */
	int32 TotalRoomToFill{ 0 };

	/** Quantity that could not fit (overflow). */
	int32 Remainder{ 0 };

	bool bStackable{ false };

	TArray<FFP_SlotAvailability> SlotAvailabilities;
};
