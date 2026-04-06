// Copyright JG

#include "UI/Widget/Inventory/FP_SpatialInventory.h"

FFP_SlotAvailabilityResult UFP_SpatialInventory::HasRoomForItem(UFP_ItemComponent* ItemComponent) const
{
	// Stub: always report one slot available so the pickup pipeline can be tested end-to-end.
	// Replace once item dimensions and stack rules are defined.
	FFP_SlotAvailabilityResult Result;
	Result.TotalRoomToFill = 1;
	return Result;
}
