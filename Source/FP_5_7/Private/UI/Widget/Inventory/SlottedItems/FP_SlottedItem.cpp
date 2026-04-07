// Copyright JG

#include "UI/Widget/Inventory/SlottedItems/FP_SlottedItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Inventory/InventoryManagement/Items/FP_InventoryItem.h"

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
