// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Inventory/FP_InventoryBase.h"
#include "FP_SpatialInventory.generated.h"

class UFP_InventoryGrid;
class UFP_ItemDescription;
class UCanvasPanel;

/**
 * Spatial (grid-based) inventory widget. Contains a single grid for all item categories.
 */
UCLASS()
class FP_5_7_API UFP_SpatialInventory : public UFP_InventoryBase
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FFP_SlotAvailabilityResult HasRoomForItem(UFP_ItemComponent* ItemComponent) const override;

	virtual void OnItemHovered(UFP_SlottedItem* SlottedItem) override;
	virtual void OnItemUnhovered(UFP_SlottedItem* SlottedItem) override;
	virtual bool HasHoverItem() const override;

private:

	UFP_ItemDescription* GetItemDescription();
	void SetItemDescriptionSizeAndPosition();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFP_InventoryGrid> Grid;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UFP_ItemDescription> ItemDescriptionClass;

	UPROPERTY()
	TObjectPtr<UFP_ItemDescription> ItemDescription;

	FTimerHandle ItemDescriptionTimerHandle;

	/** Delay (seconds) before the item description tooltip appears on hover. */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float ItemDescriptionDelay{ 0.5f };
};
