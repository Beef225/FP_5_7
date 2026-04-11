// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "FP_HoverItem.generated.h"

class UImage;
class UTextBlock;
class UFP_InventoryItem;

/**
 * Widget that follows the mouse cursor when an inventory item is clicked/dragged.
 * Displays the item icon and stack count, and carries metadata needed to
 * resolve where the item came from and how it fits on the grid.
 */
UCLASS()
class FP_5_7_API UFP_HoverItem : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetImageBrush(const FSlateBrush& Brush) const;
	void UpdateStackCount(int32 Count);

	FGameplayTag GetItemType() const;
	int32 GetStackCount() const { return StackCount; }
	bool IsStackable() const { return bIsStackable; }
	void SetIsStackable(bool bStacks);

	int32 GetPreviousGridIndex() const { return PreviousGridIndex; }
	void SetPreviousGridIndex(int32 Index) { PreviousGridIndex = Index; }

	FIntPoint GetGridDimensions() const { return GridDimensions; }
	void SetGridDimensions(const FIntPoint& Dimensions) { GridDimensions = Dimensions; }

	UFP_InventoryItem* GetInventoryItem() const;
	void SetInventoryItem(UFP_InventoryItem* Item);

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_StackCount;

	int32 PreviousGridIndex{ 0 };
	FIntPoint GridDimensions{ 1, 1 };
	TWeakObjectPtr<UFP_InventoryItem> InventoryItem;
	bool bIsStackable{ false };
	int32 StackCount{ 0 };
};
