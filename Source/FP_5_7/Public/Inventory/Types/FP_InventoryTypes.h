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

UENUM(BlueprintType)
enum class EFP_TileQuadrant : uint8
{
	TopLeft,
	TopRight,
	BottomLeft,
	BottomRight,
	None
};

USTRUCT(BlueprintType)
struct FFP_TileParameters
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	FIntPoint TileCoordinates{};

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	int32 TileIndex{ INDEX_NONE };

	UPROPERTY(BlueprintReadWrite, EditAnywhere, Category = "Inventory")
	EFP_TileQuadrant TileQuadrant{ EFP_TileQuadrant::None };
};

inline bool operator==(const FFP_TileParameters& A, const FFP_TileParameters& B)
{
	return A.TileCoordinates == B.TileCoordinates && A.TileIndex == B.TileIndex && A.TileQuadrant == B.TileQuadrant;
}

/** Result of querying whether a region of the grid has space for a dropped item. */
USTRUCT()
struct FFP_SpaceQueryResult
{
	GENERATED_BODY()

	/** True if the queried space is completely empty. */
	bool bHasSpace{ false };

	/** Valid if there is exactly one item in the way that we could swap with. */
	TWeakObjectPtr<UFP_InventoryItem> ValidItem = nullptr;

	/** Upper-left grid index of ValidItem, if present. */
	int32 UpperLeftIndex{ INDEX_NONE };
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
