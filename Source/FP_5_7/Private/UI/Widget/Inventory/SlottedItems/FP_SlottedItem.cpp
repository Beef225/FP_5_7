// Copyright JG

#include "UI/Widget/Inventory/SlottedItems/FP_SlottedItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Inventory/InventoryManagement/Items/FP_InventoryItem.h"
#include "UI/Widget/Inventory/FP_InventoryBase.h"

FReply UFP_SlottedItem::NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent)
{
	OnSlottedItemClicked.Broadcast(GridIndex, MouseEvent);
	return FReply::Handled();
}

void UFP_SlottedItem::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	if (OwningInventory.IsValid())
	{
		OwningInventory->OnItemHovered(this);
	}
}

void UFP_SlottedItem::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	if (OwningInventory.IsValid())
	{
		OwningInventory->OnItemUnhovered(this);
	}
}

void UFP_SlottedItem::SetInventoryItem(UFP_InventoryItem* Item)
{
	InventoryItem = Item;
}

void UFP_SlottedItem::SetImageBrush(const FSlateBrush& Brush) const
{
	Image_Icon->SetBrush(Brush);
}

void UFP_SlottedItem::UpdateStackCount(int32 StackCount)
{
	if (StackCount > 0)
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Visible);
		Text_StackCount->SetText(FText::AsNumber(StackCount));
	}
	else
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Collapsed);
	}
}
