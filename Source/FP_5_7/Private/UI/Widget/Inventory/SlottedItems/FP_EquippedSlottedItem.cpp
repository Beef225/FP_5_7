// Copyright JG

#include "UI/Widget/Inventory/SlottedItems/FP_EquippedSlottedItem.h"

FReply UFP_EquippedSlottedItem::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	OnEquippedSlottedItemClicked.Broadcast(this);
	return FReply::Handled();
}
