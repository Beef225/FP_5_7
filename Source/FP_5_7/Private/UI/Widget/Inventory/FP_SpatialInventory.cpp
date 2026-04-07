// Copyright JG

#include "UI/Widget/Inventory/FP_SpatialInventory.h"
#include "Libraries/FP_AbilitySystemLibrary.h"
#include "Libraries/FP_EnumDefs.h"
#include "UI/Widget/Inventory/FP_InventoryGrid.h"

FFP_SlotAvailabilityResult UFP_SpatialInventory::HasRoomForItem(UFP_ItemComponent* ItemComponent) const
{
	const EItemCategory Category = UFP_AbilitySystemLibrary::GetItemCategoryFromItemComp(ItemComponent);

	if (Category == EItemCategory::None)
	{
		UE_LOG(LogTemp, Error, TEXT("UFP_SpatialInventory::HasRoomForItem — ItemComponent has no valid ItemCategory."));
		return FFP_SlotAvailabilityResult();
	}

	// Single grid handles all categories — route directly.
	return Grid->HasRoomForItem(ItemComponent);
}
