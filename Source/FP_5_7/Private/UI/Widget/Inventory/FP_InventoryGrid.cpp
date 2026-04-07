// Copyright JG

#include "UI/Widget/Inventory/FP_InventoryGrid.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "FP_GameplayTags.h"
#include "Inventory/InventoryManagement/Components/FP_InventoryComponent.h"
#include "Inventory/InventoryManagement/Items/FP_InventoryItem.h"
#include "Inventory/Items/FP_ItemComponent.h"
#include "Inventory/Items/Fragments/FP_ItemFragment.h"
#include "Inventory/Items/Manifest/FP_ItemManifest.h"
#include "Libraries/FP_AbilitySystemLibrary.h"
#include "Libraries/FP_WidgetUtils.h"
#include "UI/Widget/Inventory/GridSlots/FP_GridSlot.h"
#include "UI/Widget/Inventory/SlottedItems/FP_SlottedItem.h"

void UFP_InventoryGrid::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	APlayerController* PC = GetOwningPlayer();
	InventoryComponent = UFP_AbilitySystemLibrary::GetInventoryComponent(PC);

	if (InventoryComponent.IsValid())
	{
		InventoryComponent->OnItemAdded.AddDynamic(this, &ThisClass::AddItem);
	}

	ConstructGrid();
}

FFP_SlotAvailabilityResult UFP_InventoryGrid::HasRoomForItem(const UFP_ItemComponent* ItemComponent)
{
	return HasRoomForItem(ItemComponent->GetItemManifest());
}

FFP_SlotAvailabilityResult UFP_InventoryGrid::HasRoomForItem(const UFP_InventoryItem* Item)
{
	return HasRoomForItem(Item->GetItemManifest());
}

FFP_SlotAvailabilityResult UFP_InventoryGrid::HasRoomForItem(const FFP_ItemManifest& Manifest)
{
	// Stub: hardcoded test data to exercise the stacking display.
	// Replace once spatial slot logic (item dimensions, occupancy tracking) is implemented.
	FFP_SlotAvailabilityResult Result;
	Result.TotalRoomToFill = 7;
	Result.bStackable = true;

	FFP_SlotAvailability SlotAvailability;
	SlotAvailability.AmountToFill = 2;
	SlotAvailability.Index = 0;
	Result.SlotAvailabilities.Add(MoveTemp(SlotAvailability));

	FFP_SlotAvailability SlotAvailability2;
	SlotAvailability2.AmountToFill = 5;
	SlotAvailability2.Index = 1;
	Result.SlotAvailabilities.Add(MoveTemp(SlotAvailability2));

	return Result;
}

void UFP_InventoryGrid::AddItem(UFP_InventoryItem* Item)
{
	if (!IsValid(Item)) return;

	const FFP_SlotAvailabilityResult Result = HasRoomForItem(Item);
	AddItemToIndices(Result, Item);
}

void UFP_InventoryGrid::AddItemToIndices(const FFP_SlotAvailabilityResult& Result, UFP_InventoryItem* NewItem)
{
	for (const FFP_SlotAvailability& Availability : Result.SlotAvailabilities)
	{
		AddItemAtIndex(NewItem, Availability.Index, Result.bStackable, Availability.AmountToFill);
		UpdateGridSlots(NewItem, Availability.Index, Result.bStackable, Availability.AmountToFill);
	}
}

void UFP_InventoryGrid::AddItemAtIndex(UFP_InventoryItem* Item, const int32 Index, const bool bStackable, const int32 StackAmount)
{
	const FFP_GridFragment* GridFragment = GetFragment<FFP_GridFragment>(Item, FFP_GameplayTags::Get().Fragment_Grid);
	const FFP_ImageFragment* ImageFragment = GetFragment<FFP_ImageFragment>(Item, FFP_GameplayTags::Get().Fragment_Icon);
	if (!GridFragment || !ImageFragment) return;

	UFP_SlottedItem* SlottedItem = CreateSlottedItem(Item, bStackable, StackAmount, GridFragment, ImageFragment, Index);

	AddSlottedItemToCanvas(Index, GridFragment, SlottedItem);
	SlottedItems.Add(Index, SlottedItem);
}

void UFP_InventoryGrid::AddSlottedItemToCanvas(const int32 Index, const FFP_GridFragment* GridFragment, UFP_SlottedItem* SlottedItem) const
{
	CanvasPanel->AddChild(SlottedItem);
	UCanvasPanelSlot* CanvasSlot = UWidgetLayoutLibrary::SlotAsCanvasSlot(SlottedItem);
	CanvasSlot->SetSize(GetDrawSize(GridFragment));
	const FVector2D DrawPos = FVector2D(UFP_WidgetUtils::GetPositionFromIndex(Index, Columns)) * TileSize;
	const FVector2D DrawPosWithPadding = DrawPos + FVector2D(GridFragment->GetGridPadding());
	CanvasSlot->SetPosition(DrawPosWithPadding);
}

UFP_SlottedItem* UFP_InventoryGrid::CreateSlottedItem(UFP_InventoryItem* Item, const bool bStackable,
	const int32 StackAmount, const FFP_GridFragment* GridFragment, const FFP_ImageFragment* ImageFragment,
	const int32 Index)
{
	UFP_SlottedItem* SlottedItem = CreateWidget<UFP_SlottedItem>(GetOwningPlayer(), SlottedItemClass);
	SlottedItem->SetInventoryItem(Item);
	SlottedItem->SetIsStackable(bStackable);
	SlottedItem->SetGridIndex(Index);
	SlottedItem->SetGridDimensions(GridFragment->GetGridSize());
	SetSlottedItemImage(SlottedItem, GridFragment, ImageFragment);
	const int32 StackUpdateAmount = bStackable ? StackAmount : 0;
	SlottedItem->UpdateStackCount(StackUpdateAmount);
	return SlottedItem;
}

void UFP_InventoryGrid::UpdateGridSlots(UFP_InventoryItem* NewItem, const int32 Index, const bool bStackableItem, const int32 StackAmount)
{
	check(GridSlots.IsValidIndex(Index));

	if (bStackableItem)
	{
		GridSlots[Index]->SetStackCount(StackAmount);
	}

	const FFP_GridFragment* GridFragment = GetFragment<FFP_GridFragment>(NewItem, FFP_GameplayTags::Get().Fragment_Grid);
	const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint(1, 1);

	UFP_AbilitySystemLibrary::ForEach2D(GridSlots, Index, Dimensions, Columns, [&](UFP_GridSlot* GridSlot)
	{
		GridSlot->SetInventoryItem(NewItem);
		GridSlot->SetUpperLeftIndex(Index);
		GridSlot->SetOccupiedTexture();
		GridSlot->SetAvailable(false);
	});
}

FVector2D UFP_InventoryGrid::GetDrawSize(const FFP_GridFragment* GridFragment) const
{
	const float IconTileWidth = TileSize - GridFragment->GetGridPadding() * 2.f;
	return FVector2D(GridFragment->GetGridSize()) * IconTileWidth;
}

void UFP_InventoryGrid::SetSlottedItemImage(const UFP_SlottedItem* SlottedItem,
	const FFP_GridFragment* GridFragment, const FFP_ImageFragment* ImageFragment) const
{
	FSlateBrush Brush;
	Brush.SetResourceObject(ImageFragment->GetIcon());
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = GetDrawSize(GridFragment);
	SlottedItem->SetImageBrush(Brush);
}

void UFP_InventoryGrid::ConstructGrid()
{
	GridSlots.Reserve(Rows * Columns);

	for (int32 j = 0; j < Rows; ++j)
	{
		for (int32 i = 0; i < Columns; ++i)
		{
			UFP_GridSlot* GridSlot = CreateWidget<UFP_GridSlot>(this, GridSlotClass);
			CanvasPanel->AddChild(GridSlot);

			const FIntPoint TilePosition(i, j);
			GridSlot->SetTileIndex(UFP_WidgetUtils::GetIndexFromPosition(TilePosition, Columns));

			UCanvasPanelSlot* GridCPS = UWidgetLayoutLibrary::SlotAsCanvasSlot(GridSlot);
			GridCPS->SetSize(FVector2D(TileSize));
			GridCPS->SetPosition(FVector2D(TilePosition) * TileSize);

			GridSlots.Add(GridSlot);
		}
	}
}
