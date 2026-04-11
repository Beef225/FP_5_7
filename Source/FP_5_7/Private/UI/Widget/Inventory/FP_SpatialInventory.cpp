// Copyright JG

#include "UI/Widget/Inventory/FP_SpatialInventory.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Input/Reply.h"
#include "Libraries/FP_AbilitySystemLibrary.h"
#include "Libraries/FP_EnumDefs.h"
#include "Libraries/FP_WidgetUtils.h"
#include "UI/Widget/Inventory/FP_InventoryGrid.h"
#include "UI/Widget/Items/FP_ItemDescription.h"

void UFP_SpatialInventory::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (IsValid(Grid))
	{
		Grid->ShowCursor();
		Grid->SetOwningInventoryBase(this);
	}
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
	GetItemDescription()->SetVisibility(ESlateVisibility::Collapsed);
	GetWorld()->GetTimerManager().ClearTimer(ItemDescriptionTimerHandle);

	GetWorld()->GetTimerManager().SetTimer(ItemDescriptionTimerHandle, [this]()
	{
		GetItemDescription()->SetVisibility(ESlateVisibility::Visible);
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
