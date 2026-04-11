// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Inventory/FP_InventoryBase.h"
#include "FP_SpatialInventory.generated.h"

class UFP_InventoryGrid;

/**
 * Spatial (grid-based) inventory widget. Contains a single grid for all item categories.
 */
UCLASS()
class FP_5_7_API UFP_SpatialInventory : public UFP_InventoryBase
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized() override;
	virtual FFP_SlotAvailabilityResult HasRoomForItem(UFP_ItemComponent* ItemComponent) const override;

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFP_InventoryGrid> Grid;
};
