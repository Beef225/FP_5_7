// Copyright JG

#include "UI/Widget/Inventory/FP_SpatialInventory.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Blueprint/WidgetTree.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Input/Reply.h"
#include "Inventory/InventoryManagement/Items/FP_InventoryItem.h"
#include "Libraries/FP_AbilitySystemLibrary.h"
#include "Libraries/FP_EnumDefs.h"
#include "Libraries/FP_WidgetUtils.h"
#include "UI/Widget/Inventory/FP_InventoryGrid.h"
#include "Inventory/InventoryManagement/Components/FP_InventoryComponent.h"
#include "UI/Widget/Inventory/GridSlots/FP_EquippedGridSlot.h"
#include "UI/Widget/Inventory/SlottedItems/FP_EquippedSlottedItem.h"
#include "UI/Widget/Inventory/SlottedItems/FP_SlottedItem.h"
#include "UI/Widget/Inventory/HoverItem/FP_HoverItem.h"
#include "UI/Widget/Items/FP_ItemDescription.h"

void UFP_SpatialInventory::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (IsValid(Grid))
	{
		Grid->ShowCursor();
		Grid->SetOwningInventoryBase(this);
	}

	// Discover all UFP_EquippedGridSlot widgets placed in the Blueprint layout and bind their delegates.
	WidgetTree->ForEachWidget([this](UWidget* Widget)
	{
		if (UFP_EquippedGridSlot* EquippedSlot = Cast<UFP_EquippedGridSlot>(Widget))
		{
			EquippedGridSlots.Add(EquippedSlot);
			EquippedSlot->EquippedGridSlotClicked.AddDynamic(this, &ThisClass::EquippedGridSlotClicked);
		}
	});
}

void UFP_SpatialInventory::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (IsValid(ItemDescription) && ItemDescription->IsVisible())
	{
		SetItemDescriptionSizeAndPosition();
	}
}

FReply UFP_SpatialInventory::NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.IsMouseButtonDown(EKeys::LeftMouseButton) && IsValid(Grid) && Grid->HasHoverItem())
	{
		Grid->DropItem();
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FFP_SlotAvailabilityResult UFP_SpatialInventory::HasRoomForItem(UFP_ItemComponent* ItemComponent) const
{
	const EItemCategory Category = UFP_AbilitySystemLibrary::GetItemCategoryFromItemComp(ItemComponent);

	if (Category == EItemCategory::None)
	{
		UE_LOG(LogTemp, Error, TEXT("UFP_SpatialInventory::HasRoomForItem — ItemComponent has no valid ItemCategory."));
		return FFP_SlotAvailabilityResult();
	}

	// Single grid handles all categories — route directly.
	return Grid->HasRoomForItem(ItemComponent);
}

bool UFP_SpatialInventory::HasHoverItem() const
{
	return IsValid(Grid) && Grid->HasHoverItem();
}

void UFP_SpatialInventory::OnItemHovered(UFP_SlottedItem* SlottedItem)
{
	UFP_ItemDescription* DescWidget = GetItemDescription();
	DescWidget->SetVisibility(ESlateVisibility::Collapsed);
	GetWorld()->GetTimerManager().ClearTimer(ItemDescriptionTimerHandle);

	TWeakObjectPtr<UFP_InventoryItem> WeakItem = SlottedItem ? SlottedItem->GetInventoryItem() : nullptr;
	GetWorld()->GetTimerManager().SetTimer(ItemDescriptionTimerHandle, [this, WeakItem, DescWidget]()
	{
		if (WeakItem.IsValid())
		{
			WeakItem->GetItemManifest().AssimilateInventoryFragments(DescWidget);
		}
		DescWidget->SetVisibility(ESlateVisibility::HitTestInvisible);
	}, ItemDescriptionDelay, false);
}

void UFP_SpatialInventory::OnItemUnhovered(UFP_SlottedItem* SlottedItem)
{
	GetItemDescription()->SetVisibility(ESlateVisibility::Collapsed);
	GetWorld()->GetTimerManager().ClearTimer(ItemDescriptionTimerHandle);
}

UFP_ItemDescription* UFP_SpatialInventory::GetItemDescription()
{
	if (!IsValid(ItemDescription))
	{
		ItemDescription = CreateWidget<UFP_ItemDescription>(GetOwningPlayer(), ItemDescriptionClass);
		ItemDescription->SetVisibility(ESlateVisibility::Collapsed);

		UCanvasPanelSlot* CanvasSlot = CanvasPanel->AddChildToCanvas(ItemDescription);
		CanvasSlot->SetAutoSize(true);
	}
	return ItemDescription;
}

UFP_HoverItem* UFP_SpatialInventory::GetHoverItem() const
{
	if (!IsValid(Grid)) return nullptr;
	return Grid->GetHoverItem();
}

void UFP_SpatialInventory::EquippedGridSlotClicked(UFP_EquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentTypeTag)
{
	if (!CanEquipHoverItem(EquippedGridSlot, EquipmentTypeTag)) return;

	UFP_HoverItem* HoverItem = GetHoverItem();
	UFP_InventoryItem* ItemToEquip = HoverItem->GetInventoryItem();

	UFP_EquippedSlottedItem* EquippedSlottedItem = EquippedGridSlot->OnItemEquipped(
		ItemToEquip,
		EquipmentTypeTag,
		Grid->GetTileSize()
	);
	EquippedSlottedItem->OnEquippedSlottedItemClicked.AddDynamic(this, &ThisClass::EquippedSlottedItemClicked);

	Grid->ClearHoverItem();

	UFP_InventoryComponent* InventoryComponent = UFP_AbilitySystemLibrary::GetInventoryComponent(GetOwningPlayer());
	check(IsValid(InventoryComponent));

	InventoryComponent->Server_EquipSlotClicked(ItemToEquip, nullptr);

	if (GetOwningPlayer()->GetNetMode() != NM_DedicatedServer)
	{
		InventoryComponent->OnItemEquipped.Broadcast(ItemToEquip);
	}
}

void UFP_SpatialInventory::EquippedSlottedItemClicked(UFP_EquippedSlottedItem* EquippedSlottedItem)
{
	// Remove the Item Description
	OnItemUnhovered(nullptr);

	if (IsValid(GetHoverItem()) && GetHoverItem()->IsStackable()) return;

	// Get Item to Equip
	UFP_InventoryItem* ItemToEquip = IsValid(GetHoverItem()) ? GetHoverItem()->GetInventoryItem() : nullptr;

	// Get Item to Unequip
	UFP_InventoryItem* ItemToUnequip = EquippedSlottedItem->GetInventoryItem();

	// Get the Equipped Grid Slot holding this item
	UFP_EquippedGridSlot* EquippedGridSlot = FindSlotWithEquippedItem(ItemToUnequip);

	// Clear the equipped grid slot of this item (set its inventory item to nullptr)
	ClearSlotOfItem(EquippedGridSlot);

	// Assign previously equipped item as the hover item
	Grid->AssignHoverItem(ItemToUnequip);

	// Remove the equipped slotted item from the equipped grid slot
	RemoveEquippedSlottedItem(EquippedSlottedItem);

	// Make a new equipped slotted item (for the item we held in HoverItem)
	MakeEquippedSlottedItem(EquippedSlottedItem, EquippedGridSlot, ItemToEquip);

	// Broadcast delegates for OnItemEquipped/OnItemUnequipped (from the IC)
	BroadcastSlotClickedDelegates(ItemToEquip, ItemToUnequip);
}

void UFP_SpatialInventory::ClearSlotOfItem(UFP_EquippedGridSlot* EquippedGridSlot)
{
	if (IsValid(EquippedGridSlot))
	{
		EquippedGridSlot->SetEquippedSlottedItem(nullptr);
		EquippedGridSlot->SetInventoryItem(nullptr);
	}
}

void UFP_SpatialInventory::RemoveEquippedSlottedItem(UFP_EquippedSlottedItem* EquippedSlottedItem)
{
	if (!IsValid(EquippedSlottedItem)) return;

	if (EquippedSlottedItem->OnEquippedSlottedItemClicked.IsAlreadyBound(this, &ThisClass::EquippedSlottedItemClicked))
	{
		EquippedSlottedItem->OnEquippedSlottedItemClicked.RemoveDynamic(this, &ThisClass::EquippedSlottedItemClicked);
	}
	EquippedSlottedItem->RemoveFromParent();
}

void UFP_SpatialInventory::MakeEquippedSlottedItem(UFP_EquippedSlottedItem* EquippedSlottedItem, UFP_EquippedGridSlot* EquippedGridSlot, UFP_InventoryItem* ItemToEquip)
{
	if (!IsValid(EquippedGridSlot)) return;

	UFP_EquippedSlottedItem* SlottedItem = EquippedGridSlot->OnItemEquipped(
		ItemToEquip,
		EquippedSlottedItem->GetEquipmentTypeTag(),
		Grid->GetTileSize());
	if (IsValid(SlottedItem)) SlottedItem->OnEquippedSlottedItemClicked.AddDynamic(this, &ThisClass::EquippedSlottedItemClicked);

	EquippedGridSlot->SetEquippedSlottedItem(SlottedItem);
}

UFP_EquippedGridSlot* UFP_SpatialInventory::FindSlotWithEquippedItem(UFP_InventoryItem* EquippedItem) const
{
	const TObjectPtr<UFP_EquippedGridSlot>* FoundSlot = EquippedGridSlots.FindByPredicate(
		[EquippedItem](const UFP_EquippedGridSlot* GridSlot)
		{
			return GridSlot->GetInventoryItem() == EquippedItem;
		});
	return FoundSlot ? *FoundSlot : nullptr;
}

void UFP_SpatialInventory::BroadcastSlotClickedDelegates(UFP_InventoryItem* ItemToEquip, UFP_InventoryItem* ItemToUnequip) const
{
	UFP_InventoryComponent* InventoryComponent = UFP_AbilitySystemLibrary::GetInventoryComponent(GetOwningPlayer());
	check(IsValid(InventoryComponent));

	InventoryComponent->Server_EquipSlotClicked(ItemToEquip, ItemToUnequip);

	if (GetOwningPlayer()->GetNetMode() != NM_DedicatedServer)
	{
		InventoryComponent->OnItemEquipped.Broadcast(ItemToEquip);
		InventoryComponent->OnItemUnequipped.Broadcast(ItemToUnequip);
	}
}

bool UFP_SpatialInventory::CanEquipHoverItem(UFP_EquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentTypeTag) const
{
	if (!IsValid(EquippedGridSlot) || EquippedGridSlot->GetInventoryItem().IsValid()) return false;

	UFP_HoverItem* HoverItem = GetHoverItem();
	if (!IsValid(HoverItem)) return false;

	UFP_InventoryItem* HeldItem = HoverItem->GetInventoryItem();

	return HasHoverItem() && IsValid(HeldItem) &&
		!HoverItem->IsStackable() &&
			HeldItem->GetItemManifest().GetItemCategory() == EItemCategory::Equippable &&
				HeldItem->GetItemManifest().GetItemType().MatchesTag(EquipmentTypeTag);
}

void UFP_SpatialInventory::SetItemDescriptionSizeAndPosition()
{
	UCanvasPanelSlot* CanvasSlot = Cast<UCanvasPanelSlot>(ItemDescription->Slot);
	if (!CanvasSlot) return;

	const FVector2D DescSize = ItemDescription->GetBoxSize();
	CanvasSlot->SetSize(DescSize);

	const FVector2D MousePos = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());
	const FVector2D CanvasSize = UFP_WidgetUtils::GetWidgetSize(CanvasPanel);
	const FVector2D ClampedPos = UFP_WidgetUtils::GetClampedWidgetPosition(CanvasSize, DescSize, MousePos);
	CanvasSlot->SetPosition(ClampedPos);
}
