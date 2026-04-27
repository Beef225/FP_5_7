// Copyright JG

#include "UI/Widget/Inventory/GridSlots/FP_EquippedGridSlot.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/Image.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "FP_GameplayTags.h"
#include "Inventory/InventoryManagement/Items/FP_InventoryItem.h"
#include "Inventory/Items/Fragments/FP_ItemFragment.h"
#include "Libraries/FP_AbilitySystemLibrary.h"
#include "UI/Widget/Inventory/HoverItem/FP_HoverItem.h"
#include "UI/Widget/Inventory/SlottedItems/FP_EquippedSlottedItem.h"

void UFP_EquippedGridSlot::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);

	if (!IsAvailable()) return;

	UFP_HoverItem* HoverItem = UFP_AbilitySystemLibrary::GetHoverItem(GetOwningPlayer());
	if (!IsValid(HoverItem)) return;

	if (HoverItem->GetItemType().MatchesTag(EquipmentTypeTag))
	{
		Image_GrayedOutIcon->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UFP_EquippedGridSlot::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);

	// Only undo the hover highlight if we actually applied it on enter.
	UFP_HoverItem* HoverItem = UFP_AbilitySystemLibrary::GetHoverItem(GetOwningPlayer());
	if (!IsValid(HoverItem) || !HoverItem->GetItemType().MatchesTag(EquipmentTypeTag))
		return;

	if (!GetInventoryItem().IsValid())
		Image_GrayedOutIcon->SetVisibility(ESlateVisibility::Visible);
}

void UFP_EquippedGridSlot::ResetVisuals()
{
	Image_GrayedOutIcon->SetVisibility(ESlateVisibility::Visible);
}

FReply UFP_EquippedGridSlot::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	EquippedGridSlotClicked.Broadcast(this, EquipmentTypeTag);
	return FReply::Handled();
}

UFP_EquippedSlottedItem* UFP_EquippedGridSlot::OnItemEquipped(UFP_InventoryItem* Item, const FGameplayTag& EquipmentTag, float TileSize)
{
	if (!EquipmentTag.MatchesTagExact(EquipmentTypeTag)) return nullptr;

	const FFP_GridFragment* GridFragment = Item->GetItemManifest().GetFragmentOfType<FFP_GridFragment>();
	if (!GridFragment) return nullptr;

	const FIntPoint GridDimensions = GridFragment->GetGridSize();

	const float IconTileWidth = TileSize - GridFragment->GetGridPadding() * 2;
	const FVector2D DrawSize = FVector2D(GridDimensions.X, GridDimensions.Y) * IconTileWidth;

	EquippedSlottedItem = CreateWidget<UFP_EquippedSlottedItem>(GetOwningPlayer(), EquippedSlottedItemClass);

	EquippedSlottedItem->SetInventoryItem(Item);
	EquippedSlottedItem->SetEquipmentTypeTag(EquipmentTag);
	EquippedSlottedItem->UpdateStackCount(0);

	SetInventoryItem(Item);

	const FFP_ImageFragment* ImageFragment = Item->GetItemManifest().GetFragmentOfType<FFP_ImageFragment>();
	if (!ImageFragment) return nullptr;

	FSlateBrush Brush;
	Brush.SetResourceObject(ImageFragment->GetIcon());
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = DrawSize;
	EquippedSlottedItem->SetImageBrush(Brush);

	Overlay_Root->AddChildToOverlay(EquippedSlottedItem);

	UOverlaySlot* OverlaySlot = UWidgetLayoutLibrary::SlotAsOverlaySlot(EquippedSlottedItem);
	OverlaySlot->SetHorizontalAlignment(HAlign_Center);
	OverlaySlot->SetVerticalAlignment(VAlign_Center);

	Image_GrayedOutIcon->SetVisibility(ESlateVisibility::Collapsed);

	return EquippedSlottedItem;
}
