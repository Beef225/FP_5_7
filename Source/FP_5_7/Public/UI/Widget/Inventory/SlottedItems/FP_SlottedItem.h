// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "UI/Widget/Inventory/FP_InventoryBase.h"
#include "FP_SlottedItem.generated.h"

class UImage;
class UTextBlock;
class UFP_InventoryItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FFP_SlottedItemClicked, int32, GridIndex, const FPointerEvent&, MouseEvent);

/**
 * Widget representing a single item placed on the inventory grid.
 * Holds a reference to the backing UFP_InventoryItem, the grid index it occupies,
 * its tile dimensions, and the icon image.
 */
UCLASS()
class FP_5_7_API UFP_SlottedItem : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	void SetOwningInventory(UFP_InventoryBase* Inventory) { OwningInventory = Inventory; }

	FFP_SlottedItemClicked OnSlottedItemClicked;

	bool IsStackable() const { return bIsStackable; }
	void SetIsStackable(bool bStackable) { bIsStackable = bStackable; }

	UImage* GetImageIcon() const { return Image_Icon; }

	void SetGridIndex(int32 Index) { GridIndex = Index; }
	int32 GetGridIndex() const { return GridIndex; }

	void SetGridDimensions(const FIntPoint& Dimensions) { GridDimensions = Dimensions; }
	FIntPoint GetGridDimensions() const { return GridDimensions; }

	void SetInventoryItem(UFP_InventoryItem* Item);
	UFP_InventoryItem* GetInventoryItem() const { return InventoryItem.Get(); }

	void SetImageBrush(const FSlateBrush& Brush) const;
	void UpdateStackCount(int32 StackCount);

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_StackCount;

	int32 GridIndex{ 0 };
	FIntPoint GridDimensions{ 1, 1 };
	TWeakObjectPtr<UFP_InventoryItem> InventoryItem;
	TWeakObjectPtr<UFP_InventoryBase> OwningInventory;
	bool bIsStackable{ false };
};
