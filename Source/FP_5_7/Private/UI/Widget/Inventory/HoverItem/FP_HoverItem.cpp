// Copyright JG

#include "UI/Widget/Inventory/HoverItem/FP_HoverItem.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Inventory/InventoryManagement/Items/FP_InventoryItem.h"

void UFP_HoverItem::SetImageBrush(const FSlateBrush& Brush) const
{
	Image_Icon->SetBrush(Brush);
}

void UFP_HoverItem::UpdateStackCount(const int32 Count)
{
	StackCount = Count;
	if (Count > 0)
	{
		Text_StackCount->SetText(FText::AsNumber(Count));
		Text_StackCount->SetVisibility(ESlateVisibility::Visible);
	}
	else
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Collapsed);
	}
}

FGameplayTag UFP_HoverItem::GetItemType() const
{
	if (InventoryItem.IsValid())
	{
		return InventoryItem->GetItemManifest().GetItemType();
	}
	return FGameplayTag();
}

void UFP_HoverItem::SetIsStackable(bool bStacks)
{
	bIsStackable = bStacks;
	if (!bStacks)
	{
		Text_StackCount->SetVisibility(ESlateVisibility::Collapsed);
	}
}

UFP_InventoryItem* UFP_HoverItem::GetInventoryItem() const
{
	return InventoryItem.Get();
}

void UFP_HoverItem::SetInventoryItem(UFP_InventoryItem* Item)
{
	InventoryItem = Item;
}
