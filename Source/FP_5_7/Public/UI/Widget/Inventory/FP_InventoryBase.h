// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/Types/FP_InventoryTypes.h"
#include "FP_InventoryBase.generated.h"

class UFP_ItemComponent;
class UFP_SlottedItem;

/**
 * Base class for all inventory UI widgets.
 */
UCLASS()
class FP_5_7_API UFP_InventoryBase : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual FFP_SlotAvailabilityResult HasRoomForItem(UFP_ItemComponent* ItemComponent) const
	{
		return FFP_SlotAvailabilityResult();
	}

	virtual void OnItemHovered(UFP_SlottedItem* SlottedItem) {}
	virtual void OnItemUnhovered(UFP_SlottedItem* SlottedItem) {}
	virtual bool HasHoverItem() const { return false; }
};
