// Copyright JG

#include "UI/Widget/Inventory/FP_InventoryGrid.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "UI/Widget/Inventory/FP_InventoryBase.h"
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
#include "UI/Widget/Inventory/HoverItem/FP_HoverItem.h"
#include "UI/Widget/Inventory/SlottedItems/FP_SlottedItem.h"
#include "UI/Widget/Items/FP_SplitStackWidget.h"

void UFP_InventoryGrid::NativeOnInitialized()
{
	Super::NativeOnInitialized();

	APlayerController* PC = GetOwningPlayer();
	InventoryComponent = UFP_AbilitySystemLibrary::GetInventoryComponent(PC);

	if (InventoryComponent.IsValid())
	{
		InventoryComponent->OnItemAdded.AddDynamic(this, &ThisClass::AddItem);
		InventoryComponent->OnStackChange.AddDynamic(this, &ThisClass::AddStacks);
		InventoryComponent->OnInventoryMenuToggled.AddDynamic(this, &ThisClass::OnInventoryMenuToggled);
	}

	ConstructGrid();
}

void UFP_InventoryGrid::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	const FVector2D CanvasPosition = UFP_WidgetUtils::GetWidgetPosition(CanvasPanel);
	const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());

	if (CursorExitedCanvas(CanvasPosition, UFP_WidgetUtils::GetWidgetSize(CanvasPanel), MousePosition))
	{
		return;
	}

	UpdateTileParameters(CanvasPosition, MousePosition);
}

void UFP_InventoryGrid::UpdateTileParameters(const FVector2D& CanvasPosition, const FVector2D& MousePosition)
{
	if (!bMouseWithinCanvas) return;

	// Calculate the tile quadrant, tile index, and coordinates
	const FIntPoint HoveredTileCoordinates = CalculateHoveredCoordinates(CanvasPosition, MousePosition);

	LastTileParameters = TileParameters;
	TileParameters.TileCoordinates = HoveredTileCoordinates;
	TileParameters.TileIndex = UFP_WidgetUtils::GetIndexFromPosition(HoveredTileCoordinates, Columns);
	TileParameters.TileQuadrant = CalculateTileQuadrant(CanvasPosition, MousePosition);

	OnTileParametersUpdated(TileParameters);
}

void UFP_InventoryGrid::OnTileParametersUpdated(const FFP_TileParameters& Parameters)
{
	if (!IsValid(HoverItem)) return;

	const FIntPoint Dimensions = HoverItem->GetGridDimensions();

	const FIntPoint StartingCoordinate = CalculateStartingCoordinate(
		Parameters.TileCoordinates, Dimensions, Parameters.TileQuadrant);

	ItemDropIndex = UFP_WidgetUtils::GetIndexFromPosition(StartingCoordinate, Columns);
	CurrentQueryResult = CheckHoverPosition(StartingCoordinate, Dimensions);

	if (CurrentQueryResult.bHasSpace)
	{
		HighlightSlots(ItemDropIndex, Dimensions);
		return;
	}
	UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);

	if (CurrentQueryResult.ValidItem.IsValid() && GridSlots.IsValidIndex(CurrentQueryResult.UpperLeftIndex))
	{
		const FFP_GridFragment* GridFragment = CurrentQueryResult.ValidItem->GetItemManifest().GetFragmentOfType<FFP_GridFragment>();
		if (!GridFragment) return;

		ChangeHoverType(CurrentQueryResult.UpperLeftIndex, GridFragment->GetGridSize(), EFP_GridSlotState::GrayedOut);
	}

	UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
}

void UFP_InventoryGrid::HighlightSlots(const int32 Index, const FIntPoint& Dimensions)
{
	if (!bMouseWithinCanvas) return;
	UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
	UFP_AbilitySystemLibrary::ForEach2D(GridSlots, Index, Dimensions, Columns,
		[&](const TObjectPtr<UFP_GridSlot>& GridSlot)
		{
			GridSlot->SetSelectedTexture();
		});
	LastHighlightedDimensions = Dimensions;
	LastHighlightedIndex = Index;
}

void UFP_InventoryGrid::UnHighlightSlots(const int32 Index, const FIntPoint& Dimensions)
{
	UFP_AbilitySystemLibrary::ForEach2D(GridSlots, Index, Dimensions, Columns,
		[&](const TObjectPtr<UFP_GridSlot>& GridSlot)
		{
			if (GridSlot->IsAvailable())
			{
				GridSlot->SetUnoccupiedTexture();
			}
			else
			{
				GridSlot->SetOccupiedTexture();
			}
		});
}

UUserWidget* UFP_InventoryGrid::GetVisibleCursorWidget()
{
	if (!IsValid(GetOwningPlayer())) return nullptr;
	if (!IsValid(VisibleCursorWidget))
	{
		VisibleCursorWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), VisibleCursorWidgetClass);
	}
	return VisibleCursorWidget;
}

UUserWidget* UFP_InventoryGrid::GetHiddenCursorWidget()
{
	if (!IsValid(GetOwningPlayer())) return nullptr;
	if (!IsValid(HiddenCursorWidget))
	{
		HiddenCursorWidget = CreateWidget<UUserWidget>(GetOwningPlayer(), HiddenCursorWidgetClass);
	}
	return HiddenCursorWidget;
}

void UFP_InventoryGrid::ShowCursor()
{
	if (!IsValid(GetOwningPlayer())) return;
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, GetVisibleCursorWidget());
}

void UFP_InventoryGrid::HideCursor()
{
	if (!IsValid(GetOwningPlayer())) return;
	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, GetHiddenCursorWidget());
}

void UFP_InventoryGrid::ChangeHoverType(const int32 Index, const FIntPoint& Dimensions, EFP_GridSlotState GridSlotState)
{
	UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
	UFP_AbilitySystemLibrary::ForEach2D(GridSlots, Index, Dimensions, Columns,
		[State = GridSlotState](const TObjectPtr<UFP_GridSlot>& GridSlot)
		{
			switch (State)
			{
			case EFP_GridSlotState::Occupied:
				GridSlot->SetOccupiedTexture();
				break;
			case EFP_GridSlotState::Unoccupied:
				GridSlot->SetUnoccupiedTexture();
				break;
			case EFP_GridSlotState::GrayedOut:
				GridSlot->SetGrayedOutTexture();
				break;
			case EFP_GridSlotState::Selected:
				GridSlot->SetSelectedTexture();
				break;
			}
		});
	LastHighlightedIndex = Index;
	LastHighlightedDimensions = Dimensions;
}

FFP_SpaceQueryResult UFP_InventoryGrid::CheckHoverPosition(const FIntPoint& Position, const FIntPoint& Dimensions)
{
	FFP_SpaceQueryResult Result;

	// in the grid bounds?
	if (!IsInGridBounds(UFP_WidgetUtils::GetIndexFromPosition(Position, Columns), Dimensions)) return Result;

	Result.bHasSpace = true;

	// If more than one of the indices is occupied with the same item, we need to see if they all have the same upper left index.
	TSet<int32> OccupiedUpperLeftIndices;
	UFP_AbilitySystemLibrary::ForEach2D(GridSlots, UFP_WidgetUtils::GetIndexFromPosition(Position, Columns), Dimensions, Columns,
		[&](const TObjectPtr<UFP_GridSlot>& GridSlot)
		{
			if (GridSlot->GetInventoryItem().IsValid())
			{
				OccupiedUpperLeftIndices.Add(GridSlot->GetUpperLeftIndex());
				Result.bHasSpace = false;
			}
		});

	// if so, is there only one item in the way? (can we swap?)
	if (OccupiedUpperLeftIndices.Num() == 1)
	{
		const int32 Index = *OccupiedUpperLeftIndices.CreateConstIterator();
		Result.ValidItem = GridSlots[Index]->GetInventoryItem();
		Result.UpperLeftIndex = GridSlots[Index]->GetUpperLeftIndex();
	}

	return Result;
}

FIntPoint UFP_InventoryGrid::CalculateStartingCoordinate(const FIntPoint& Coordinate, const FIntPoint& Dimensions, EFP_TileQuadrant Quadrant) const
{
	const int32 HasEvenWidth  = Dimensions.X % 2 == 0 ? 1 : 0;
	const int32 HasEvenHeight = Dimensions.Y % 2 == 0 ? 1 : 0;

	FIntPoint StartingCoord;
	switch (Quadrant)
	{
	case EFP_TileQuadrant::TopLeft:
		StartingCoord.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X);
		StartingCoord.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y);
		break;
	case EFP_TileQuadrant::TopRight:
		StartingCoord.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X) + HasEvenWidth;
		StartingCoord.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y);
		break;
	case EFP_TileQuadrant::BottomLeft:
		StartingCoord.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X);
		StartingCoord.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y) + HasEvenHeight;
		break;
	case EFP_TileQuadrant::BottomRight:
		StartingCoord.X = Coordinate.X - FMath::FloorToInt(0.5f * Dimensions.X) + HasEvenWidth;
		StartingCoord.Y = Coordinate.Y - FMath::FloorToInt(0.5f * Dimensions.Y) + HasEvenHeight;
		break;
	default:
		UE_LOG(LogTemp, Error, TEXT("UFP_InventoryGrid::CalculateStartingCoordinate — Invalid Quadrant."));
		return FIntPoint(-1, -1);
	}
	return StartingCoord;
}

bool UFP_InventoryGrid::CursorExitedCanvas(const FVector2D& BoundaryPos, const FVector2D& BoundarySize, const FVector2D& Location)
{
	bLastMouseWithinCanvas = bMouseWithinCanvas;
	bMouseWithinCanvas = UFP_WidgetUtils::IsWithinBounds(BoundaryPos, BoundarySize, Location);
	if (!bMouseWithinCanvas && bLastMouseWithinCanvas)
	{
		UnHighlightSlots(LastHighlightedIndex, LastHighlightedDimensions);
		return true;
	}
	return false;
}

EFP_TileQuadrant UFP_InventoryGrid::CalculateTileQuadrant(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const
{
	const float TileLocalX = FMath::Fmod(MousePosition.X - CanvasPosition.X, TileSize);
	const float TileLocalY = FMath::Fmod(MousePosition.Y - CanvasPosition.Y, TileSize);

	const bool bIsTop  = TileLocalY < TileSize / 2.f;
	const bool bIsLeft = TileLocalX < TileSize / 2.f;

	EFP_TileQuadrant HoveredTileQuadrant{ EFP_TileQuadrant::None };
	if      (bIsTop  && bIsLeft)  HoveredTileQuadrant = EFP_TileQuadrant::TopLeft;
	else if (bIsTop  && !bIsLeft) HoveredTileQuadrant = EFP_TileQuadrant::TopRight;
	else if (!bIsTop && bIsLeft)  HoveredTileQuadrant = EFP_TileQuadrant::BottomLeft;
	else if (!bIsTop && !bIsLeft) HoveredTileQuadrant = EFP_TileQuadrant::BottomRight;

	return HoveredTileQuadrant;
}

FIntPoint UFP_InventoryGrid::CalculateHoveredCoordinates(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const
{
	return FIntPoint{
		static_cast<int32>(FMath::FloorToInt((MousePosition.X - CanvasPosition.X) / TileSize)),
		static_cast<int32>(FMath::FloorToInt((MousePosition.Y - CanvasPosition.Y) / TileSize))
	};
}

FFP_SlotAvailabilityResult UFP_InventoryGrid::HasRoomForItem(const UFP_ItemComponent* ItemComponent)
{
	return HasRoomForItem(ItemComponent->GetItemManifest());
}

FFP_SlotAvailabilityResult UFP_InventoryGrid::HasRoomForItem(const UFP_InventoryItem* Item, const int32 StackAmountOverride)
{
	return HasRoomForItem(Item->GetItemManifest(), StackAmountOverride);
}

FFP_SlotAvailabilityResult UFP_InventoryGrid::HasRoomForItem(const FFP_ItemManifest& Manifest, const int32 StackAmountOverride)
{
	FFP_SlotAvailabilityResult Result;

	// 1. Determine if the item is stackable (check for StackableFragment)
	const FFP_StackableFragment* StackableFragment = Manifest.GetFragmentOfType<FFP_StackableFragment>();
	Result.bStackable = StackableFragment != nullptr;

	// 2. If stackable, determine how much total stack room is needed (TotalRoomToFill)
	const int32 MaxStackSize = StackableFragment ? StackableFragment->GetMaxStackSize() : 1;
	int32 AmountToFill = StackableFragment ? StackableFragment->GetStackCount() : 1;
	if (StackAmountOverride != -1 && Result.bStackable)
	{
		AmountToFill = StackAmountOverride;
	}

	// 3. Iterate through each grid slot
	TSet<int32> CheckedIndices;
	for (const TObjectPtr<UFP_GridSlot>& GridSlot : GridSlots)
	{
		if (AmountToFill == 0) break;
		if (IsIndexClaimed(CheckedIndices, GridSlot->GetTileIndex())) continue;
		if (!IsInGridBounds(GridSlot->GetTileIndex(), GetItemDimensions(Manifest))) continue;

		// Can the item fit here? (i.e. is it out of grid bounds?)
		TSet<int32> TentativelyClaimed;
		if (!HasRoomAtIndex(GridSlot, GetItemDimensions(Manifest), CheckedIndices, TentativelyClaimed, Manifest.GetItemType(), MaxStackSize))
		{
			continue;
		}

		// How much to fill?
		const int32 AmountToFillInSlot = DetermineFillAmountForSlot(Result.bStackable, MaxStackSize, AmountToFill, GridSlot);
		if (AmountToFillInSlot == 0) continue;

		CheckedIndices.Append(TentativelyClaimed);

		// Update the amount left to fill
		Result.TotalRoomToFill += AmountToFillInSlot;

		const int32 SlotIndex = HasValidItem(GridSlot) ? GridSlot->GetUpperLeftIndex() : GridSlot->GetTileIndex();
		Result.SlotAvailabilities.Emplace(SlotIndex, Result.bStackable ? AmountToFillInSlot : 0, HasValidItem(GridSlot));

		AmountToFill -= AmountToFillInSlot;
		Result.Remainder = AmountToFill;

		if (AmountToFill == 0) return Result;
	}

	Result.Remainder = AmountToFill;
	return Result;
}

void UFP_InventoryGrid::PickUp(UFP_InventoryItem* ClickedInventoryItem, const int32 GridIndex)
{
	AssignHoverItem(ClickedInventoryItem, GridIndex, GridIndex);
	RemoveItemFromGrid(ClickedInventoryItem, GridIndex);
}

void UFP_InventoryGrid::AssignHoverItem(UFP_InventoryItem* InventoryItem, const int32 GridIndex, const int32 PreviousGridIndex)
{
	AssignHoverItem(InventoryItem);
	HoverItem->SetPreviousGridIndex(PreviousGridIndex);
	HoverItem->UpdateStackCount(InventoryItem->IsStackable() ? GridSlots[GridIndex]->GetStackCount() : 0);
}

void UFP_InventoryGrid::RemoveItemFromGrid(UFP_InventoryItem* InventoryItem, const int32 GridIndex)
{
	const FFP_GridFragment* GridFragment = GetFragment<FFP_GridFragment>(InventoryItem, FFP_GameplayTags::Get().Fragment_Grid);
	if (!GridFragment) return;

	UFP_AbilitySystemLibrary::ForEach2D(GridSlots, GridIndex, GridFragment->GetGridSize(), Columns, [&](const TObjectPtr<UFP_GridSlot>& GridSlot)
	{
		GridSlot->SetInventoryItem(nullptr);
		GridSlot->SetUpperLeftIndex(INDEX_NONE);
		GridSlot->SetUnoccupiedTexture();
		GridSlot->SetAvailable(true);
		GridSlot->SetStackCount(0);
	});

	if (SlottedItems.Contains(GridIndex))
	{
		TObjectPtr<UFP_SlottedItem> FoundSlottedItem;
		SlottedItems.RemoveAndCopyValue(GridIndex, FoundSlottedItem);
		FoundSlottedItem->RemoveFromParent();
	}
}

void UFP_InventoryGrid::AssignHoverItem(UFP_InventoryItem* InventoryItem)
{
	if (!HoverItemClass) return;

	const FFP_GridFragment* GridFragment = GetFragment<FFP_GridFragment>(InventoryItem, FFP_GameplayTags::Get().Fragment_Grid);
	const FFP_ImageFragment* ImageFragment = GetFragment<FFP_ImageFragment>(InventoryItem, FFP_GameplayTags::Get().Fragment_Icon);
	if (!GridFragment || !ImageFragment) return;

	HoverItem = CreateWidget<UFP_HoverItem>(GetOwningPlayer(), HoverItemClass);

	const float ViewportScale = UWidgetLayoutLibrary::GetViewportScale(GetWorld());
	FSlateBrush Brush;
	Brush.SetResourceObject(ImageFragment->GetIcon());
	Brush.DrawAs = ESlateBrushDrawType::Image;
	Brush.ImageSize = GetDrawSize(GridFragment) / ViewportScale;

	HoverItem->SetImageBrush(Brush);
	HoverItem->SetGridDimensions(GridFragment->GetGridSize());
	HoverItem->SetInventoryItem(InventoryItem);
	HoverItem->SetIsStackable(InventoryItem->IsStackable());

	GetOwningPlayer()->SetMouseCursorWidget(EMouseCursor::Default, HoverItem);
}

void UFP_InventoryGrid::OnHide()
{
	PutHoverItemBack();
}

bool UFP_InventoryGrid::IsRightClick(const FPointerEvent& MouseEvent) const
{
	return MouseEvent.GetEffectingButton() == EKeys::RightMouseButton;
}

bool UFP_InventoryGrid::IsLeftClick(const FPointerEvent& MouseEvent) const
{
	return MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton;
}

void UFP_InventoryGrid::OnSlottedItemClicked(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	// Hide item description when the player clicks an item.
	if (OwningInventoryBase.IsValid())
	{
		OwningInventoryBase->OnItemUnhovered(SlottedItems.FindRef(GridIndex));
	}

	check(GridSlots.IsValidIndex(GridIndex));
	UFP_InventoryItem* ClickedInventoryItem = GridSlots[GridIndex]->GetInventoryItem().Get();

	if (IsRightClick(MouseEvent) && !IsValid(HoverItem))
	{
		if (MouseEvent.IsControlDown())
		{
			CreateSplitStackWidget(GridIndex);
		}
		else
		{
			TryConsumeItem(GridIndex);
		}
		return;
	}

	if (!IsValid(HoverItem) && IsLeftClick(MouseEvent))
	{
		PickUp(ClickedInventoryItem, GridIndex);
		return;
	}

	// Do the hovered item and the clicked inventory item share a type, and are they stackable?
	if (IsSameStackable(ClickedInventoryItem))
	{
		const int32 ClickedStackCount = GridSlots[GridIndex]->GetStackCount();
		const FFP_StackableFragment* StackableFragment = ClickedInventoryItem->GetItemManifest().GetFragmentOfType<FFP_StackableFragment>();
		const int32 MaxStackSize = StackableFragment->GetMaxStackSize();
		const int32 RoomInClickedSlot = MaxStackSize - ClickedStackCount;
		const int32 HoveredStackCount = HoverItem->GetStackCount();

		// Should we swap their stack counts? (Room in the clicked slot == 0 && HoveredStackCount < MaxStackSize)
		if (ShouldSwapStackCounts(RoomInClickedSlot, HoveredStackCount, MaxStackSize))
		{
			SwapStackCounts(ClickedStackCount, HoveredStackCount, GridIndex);
			return;
		}
		// Should we consume the hover item's stacks? (Room in the clicked slot >= HoveredStackCount)
		if (ShouldConsumeHoverItemStacks(HoveredStackCount, RoomInClickedSlot))
		{
			ConsumeHoverItemStacks(ClickedStackCount, HoveredStackCount, GridIndex);
			return;
		}
		// Should we fill in the stacks of the clicked item? (and not consume the hover item)
		if (ShouldFillInStack(RoomInClickedSlot, HoveredStackCount))
		{
			FillInStack(RoomInClickedSlot, HoveredStackCount - RoomInClickedSlot, GridIndex);
			return;
		}
		// Clicked slot is already full - do nothing (maybe play a sound?)
		if (RoomInClickedSlot == 0)
		{
			return;
		}
	}

	// Make sure we can swap with a valid item
	if (CurrentQueryResult.ValidItem.IsValid())
	{
		// Swap with the hover item.
		SwapWithHoverItem(ClickedInventoryItem, GridIndex);
	}
}

void UFP_InventoryGrid::AddStacks(const FFP_SlotAvailabilityResult& Result)
{
	if (!Result.Item.IsValid()) return;

	for (const FFP_SlotAvailability& Availability : Result.SlotAvailabilities)
	{
		if (Availability.bItemAtIndex)
		{
			// Occupied slot — update stack count visuals on the slotted item and grid slot
			const int32 NewCount = GetStackAmount(GridSlots[Availability.Index]) + Availability.AmountToFill;

			if (UFP_SlottedItem* SlottedItem = SlottedItems.FindRef(Availability.Index))
			{
				SlottedItem->UpdateStackCount(NewCount);
			}

			GridSlots[Availability.Index]->SetStackCount(NewCount);
		}
		else
		{
			// Empty slot — place the item visually as normal
			AddItemAtIndex(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.AmountToFill);
			UpdateGridSlots(Result.Item.Get(), Availability.Index, Result.bStackable, Availability.AmountToFill);
		}
	}
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
	SlottedItem->OnSlottedItemClicked.AddDynamic(this, &ThisClass::OnSlottedItemClicked);
	SlottedItem->SetOwningInventory(OwningInventoryBase.Get());
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

bool UFP_InventoryGrid::IsIndexClaimed(const TSet<int32>& CheckedIndices, const int32 Index) const
{
	return CheckedIndices.Contains(Index);
}

bool UFP_InventoryGrid::IsInGridBounds(const int32 StartIndex, const FIntPoint& ItemDimensions) const
{
	if (!GridSlots.IsValidIndex(StartIndex)) return false;
	const int32 EndColumn = (StartIndex % Columns) + ItemDimensions.X;
	const int32 EndRow = (StartIndex / Columns) + ItemDimensions.Y;
	return EndColumn <= Columns && EndRow <= Rows;
}

bool UFP_InventoryGrid::HasRoomAtIndex(const UFP_GridSlot* GridSlot, const FIntPoint& Dimensions,
	const TSet<int32>& CheckedIndices, TSet<int32>& OutTentativelyClaimed, const FGameplayTag& ItemType, int32 MaxStackSize)
{
	// Is there room at this index? (i.e. are there other items in the way?)
	bool bHasRoomAtIndex = true;
	UFP_AbilitySystemLibrary::ForEach2D(GridSlots, GridSlot->GetTileIndex(), Dimensions, Columns, [&](const TObjectPtr<UFP_GridSlot>& SubGridSlot)
	{
		if (CheckSlotConstraints(GridSlot, SubGridSlot, CheckedIndices, OutTentativelyClaimed, ItemType, MaxStackSize))
		{
			OutTentativelyClaimed.Add(SubGridSlot->GetTileIndex());
		}
		else
		{
			bHasRoomAtIndex = false;
		}
	});
	return bHasRoomAtIndex;
}

bool UFP_InventoryGrid::CheckSlotConstraints(const UFP_GridSlot* GridSlot, const UFP_GridSlot* SubGridSlot, const TSet<int32>& CheckedIndices, TSet<int32>& OutTentativelyClaimed, const FGameplayTag& ItemType, int32 MaxStackSize) const
{
	if (IsIndexClaimed(CheckedIndices, SubGridSlot->GetTileIndex())) return false;

	if (!HasValidItem(SubGridSlot))
	{
		return true;
	}

	// Slot is occupied — only valid stacking target if this sub-slot's item anchors at GridSlot
	if (!IsUpperLeftSlot(GridSlot, SubGridSlot)) return false;

	OutTentativelyClaimed.Add(SubGridSlot->GetTileIndex());

	// If so, is this a stackable item?
	const UFP_InventoryItem* SubItem = SubGridSlot->GetInventoryItem().Get();
	if (!SubItem->IsStackable()) return false;

	// Is this item the same type as the item we're trying to add?
	if (!DoesItemTypeMatch(SubItem, ItemType)) return false;

	// If stackable, is this slot at the max stack size already?
	if (GridSlot->GetStackCount() >= MaxStackSize) return false;

	return true;
}

bool UFP_InventoryGrid::HasValidItem(const UFP_GridSlot* GridSlot) const
{
	return GridSlot->GetInventoryItem().IsValid();
}

bool UFP_InventoryGrid::IsUpperLeftSlot(const UFP_GridSlot* GridSlot, const UFP_GridSlot* SubGridSlot) const
{
	return SubGridSlot->GetUpperLeftIndex() == GridSlot->GetTileIndex();
}

bool UFP_InventoryGrid::DoesItemTypeMatch(const UFP_InventoryItem* SubItem, const FGameplayTag& ItemType) const
{
	return SubItem->GetItemManifest().GetItemType().MatchesTagExact(ItemType);
}

int32 UFP_InventoryGrid::DetermineFillAmountForSlot(const bool bStackable, const int32 MaxStackSize, const int32 AmountToFill, const UFP_GridSlot* GridSlot) const
{
	const int32 RoomInSlot = MaxStackSize - GetStackAmount(GridSlot);
	return bStackable ? FMath::Min(AmountToFill, RoomInSlot) : 1;
}

int32 UFP_InventoryGrid::GetStackAmount(const UFP_GridSlot* GridSlot) const
{
	int32 CurrentSlotStackCount = GridSlot->GetStackCount();

	// If this slot isn't the anchor, look up the real stack count from the upper-left slot.
	if (const int32 UpperLeftIndex = GridSlot->GetUpperLeftIndex(); UpperLeftIndex != INDEX_NONE)
	{
		UFP_GridSlot* UpperLeftGridSlot = GridSlots[UpperLeftIndex];
		CurrentSlotStackCount = UpperLeftGridSlot->GetStackCount();
	}

	return CurrentSlotStackCount;
}

FIntPoint UFP_InventoryGrid::GetItemDimensions(const FFP_ItemManifest& Manifest) const
{
	const FFP_GridFragment* GridFragment = Manifest.GetFragmentOfType<FFP_GridFragment>();
	return GridFragment ? GridFragment->GetGridSize() : FIntPoint(1, 1);
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

			GridSlot->GridSlotClicked.AddDynamic(this, &ThisClass::OnGridSlotClicked);
			GridSlot->GridSlotHovered.AddDynamic(this, &ThisClass::OnGridSlotHovered);
			GridSlot->GridSlotUnhovered.AddDynamic(this, &ThisClass::OnGridSlotUnhovered);

			GridSlots.Add(GridSlot);
		}
	}
}

void UFP_InventoryGrid::OnGridSlotClicked(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (!IsValid(HoverItem)) return;
	if (!GridSlots.IsValidIndex(ItemDropIndex)) return;

	if (CurrentQueryResult.ValidItem.IsValid() && GridSlots.IsValidIndex(CurrentQueryResult.UpperLeftIndex))
	{
		OnSlottedItemClicked(CurrentQueryResult.UpperLeftIndex, MouseEvent);
		return;
	}

	UFP_GridSlot* GridSlot = GridSlots[ItemDropIndex];
	if (!GridSlot->GetInventoryItem().IsValid() && CurrentQueryResult.bHasSpace)
	{
		PutDownOnIndex(ItemDropIndex);
	}
}

void UFP_InventoryGrid::PutHoverItemBack()
{
	if (!IsValid(HoverItem)) return;

	FFP_SlotAvailabilityResult Result = HasRoomForItem(HoverItem->GetInventoryItem(), HoverItem->GetStackCount());
	Result.Item = HoverItem->GetInventoryItem();

	AddStacks(Result);
	ClearHoverItem();
}

void UFP_InventoryGrid::OnInventoryMenuToggled(bool bOpen)
{
	if (!bOpen)
	{
		PutHoverItemBack();
	}
}

void UFP_InventoryGrid::DropItem()
{
	if (!IsValid(HoverItem)) return;
	if (!IsValid(HoverItem->GetInventoryItem())) return;
	if (!InventoryComponent.IsValid()) return;

	InventoryComponent->Server_DropItem(HoverItem->GetInventoryItem(), HoverItem->GetStackCount());

	ClearHoverItem();
	ShowCursor();
}

void UFP_InventoryGrid::CreateSplitStackWidget(const int32 GridIndex)
{
	UFP_InventoryItem* Item = GridSlots[GridIndex]->GetInventoryItem().Get();
	if (!IsValid(Item)) return;
	if (!Item->IsStackable()) return;

	const int32 StackCount = GridSlots[GridIndex]->GetStackCount();
	if (StackCount <= 1) return;

	if (IsValid(SplitStackWidget))
	{
		SplitStackWidget->RemoveFromParent();
	}

	SplitStackWidget = CreateWidget<UFP_SplitStackWidget>(GetOwningPlayer(), SplitStackWidgetClass);
	if (!IsValid(SplitStackWidget)) return;

	SplitStackWidget->OnSplitConfirmed.AddDynamic(this, &ThisClass::OnSplitStackConfirmed);
	SplitStackWidget->OnSplitCancelled.AddDynamic(this, &ThisClass::OnSplitStackCancelled);
	SplitStackWidget->SetupForItem(Item, StackCount, GridIndex);

	const FVector2D MousePosition = UWidgetLayoutLibrary::GetMousePositionOnViewport(GetOwningPlayer());
	SplitStackWidget->SetPositionInViewport(MousePosition - SplitWidgetOffset);
	SplitStackWidget->AddToViewport();
}

void UFP_InventoryGrid::TryConsumeItem(const int32 GridIndex)
{
	UFP_InventoryItem* Item = GridSlots[GridIndex]->GetInventoryItem().Get();
	if (!IsValid(Item)) return;
	if (!Item->IsConsumable()) return;
	if (!InventoryComponent.IsValid()) return;

	const int32 UpperLeftIndex = GridSlots[GridIndex]->GetUpperLeftIndex();
	UFP_GridSlot* UpperLeftGridSlot = GridSlots[UpperLeftIndex];
	const int32 NewStackCount = UpperLeftGridSlot->GetStackCount() - 1;

	UpperLeftGridSlot->SetStackCount(NewStackCount);
	SlottedItems.FindChecked(UpperLeftIndex)->UpdateStackCount(NewStackCount);

	InventoryComponent->Server_ConsumeItem(Item);

	if (NewStackCount <= 0)
	{
		RemoveItemFromGrid(Item, UpperLeftIndex);
	}
}

void UFP_InventoryGrid::OnSplitStackConfirmed(int32 SplitAmount, int32 GridIndex)
{
	UFP_InventoryItem* Item = GridSlots[GridIndex]->GetInventoryItem().Get();
	if (!IsValid(Item)) return;
	if (!Item->IsStackable()) return;

	const int32 UpperLeftIndex = GridSlots[GridIndex]->GetUpperLeftIndex();
	UFP_GridSlot* UpperLeftGridSlot = GridSlots[UpperLeftIndex];
	const int32 StackCount = UpperLeftGridSlot->GetStackCount();
	const int32 NewStackCount = StackCount - SplitAmount;

	UpperLeftGridSlot->SetStackCount(NewStackCount);
	SlottedItems.FindChecked(UpperLeftIndex)->UpdateStackCount(NewStackCount);

	AssignHoverItem(Item, UpperLeftIndex, UpperLeftIndex);
	HoverItem->UpdateStackCount(SplitAmount);
}

void UFP_InventoryGrid::OnSplitStackCancelled()
{
	// Nothing to do — widget already removed itself
}

void UFP_InventoryGrid::PutDownOnIndex(const int32 Index)
{
	AddItemAtIndex(HoverItem->GetInventoryItem(), Index, HoverItem->IsStackable(), HoverItem->GetStackCount());
	UpdateGridSlots(HoverItem->GetInventoryItem(), Index, HoverItem->IsStackable(), HoverItem->GetStackCount());
	ClearHoverItem();
}

void UFP_InventoryGrid::ClearHoverItem()
{
	if (!IsValid(HoverItem)) return;

	HoverItem->SetInventoryItem(nullptr);
	HoverItem->SetIsStackable(false);
	HoverItem->SetPreviousGridIndex(INDEX_NONE);
	HoverItem->UpdateStackCount(0);
	HoverItem->SetImageBrush(FSlateNoResource());

	HoverItem = nullptr;
	ShowCursor();
}

void UFP_InventoryGrid::SwapStackCounts(const int32 ClickedStackCount, const int32 HoveredStackCount, const int32 Index)
{
	UFP_GridSlot* GridSlot = GridSlots[Index];
	GridSlot->SetStackCount(HoveredStackCount);

	UFP_SlottedItem* ClickedSlottedItem = SlottedItems.FindChecked(Index);
	ClickedSlottedItem->UpdateStackCount(HoveredStackCount);

	HoverItem->UpdateStackCount(ClickedStackCount);
}

bool UFP_InventoryGrid::ShouldFillInStack(const int32 RoomInClickedSlot, const int32 HoveredStackCount) const
{
	return RoomInClickedSlot < HoveredStackCount;
}

void UFP_InventoryGrid::FillInStack(const int32 FillAmount, const int32 Remainder, const int32 Index)
{
	UFP_GridSlot* GridSlot = GridSlots[Index];
	const int32 NewStackCount = GridSlot->GetStackCount() + FillAmount;

	GridSlot->SetStackCount(NewStackCount);
	SlottedItems.FindChecked(Index)->UpdateStackCount(NewStackCount);

	HoverItem->UpdateStackCount(Remainder);
}

bool UFP_InventoryGrid::ShouldConsumeHoverItemStacks(const int32 HoveredStackCount, const int32 RoomInClickedSlot) const
{
	return RoomInClickedSlot >= HoveredStackCount;
}

void UFP_InventoryGrid::ConsumeHoverItemStacks(const int32 ClickedStackCount, const int32 HoveredStackCount, const int32 Index)
{
	const int32 NewClickedStackCount = ClickedStackCount + HoveredStackCount;

	GridSlots[Index]->SetStackCount(NewClickedStackCount);
	SlottedItems.FindChecked(Index)->UpdateStackCount(NewClickedStackCount);
	ClearHoverItem();
	ShowCursor();

	const FFP_GridFragment* GridFragment = GridSlots[Index]->GetInventoryItem()->GetItemManifest().GetFragmentOfType<FFP_GridFragment>();
	const FIntPoint Dimensions = GridFragment ? GridFragment->GetGridSize() : FIntPoint(1, 1);
	HighlightSlots(Index, Dimensions);
}

bool UFP_InventoryGrid::ShouldSwapStackCounts(const int32 RoomInClickedSlot, const int32 HoveredStackCount, const int32 MaxStackSize) const
{
	return RoomInClickedSlot == 0 && HoveredStackCount < MaxStackSize;
}

bool UFP_InventoryGrid::IsSameStackable(const UFP_InventoryItem* ClickedInventoryItem) const
{
	const bool bIsSameItem   = ClickedInventoryItem == HoverItem->GetInventoryItem();
	const bool bIsStackable  = ClickedInventoryItem->IsStackable();
	return bIsSameItem && bIsStackable
		&& HoverItem->GetItemType().MatchesTagExact(ClickedInventoryItem->GetItemManifest().GetItemType());
}

void UFP_InventoryGrid::SwapWithHoverItem(UFP_InventoryItem* ClickedInventoryItem, const int32 GridIndex)
{
	if (!IsValid(HoverItem)) return;

	UFP_InventoryItem* TempInventoryItem = HoverItem->GetInventoryItem();
	const int32 TempStackCount           = HoverItem->GetStackCount();
	const bool bTempIsStackable          = HoverItem->IsStackable();

	// Keep the same previous grid index
	AssignHoverItem(ClickedInventoryItem, GridIndex, HoverItem->GetPreviousGridIndex());
	RemoveItemFromGrid(ClickedInventoryItem, GridIndex);
	AddItemAtIndex(TempInventoryItem, ItemDropIndex, bTempIsStackable, TempStackCount);
	UpdateGridSlots(TempInventoryItem, ItemDropIndex, bTempIsStackable, TempStackCount);
}

void UFP_InventoryGrid::OnGridSlotHovered(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	if (IsValid(HoverItem)) return;

	UFP_GridSlot* GridSlot = GridSlots[GridIndex];
	if (GridSlot->IsAvailable())
	{
		GridSlot->SetSelectedTexture();
	}
}

void UFP_InventoryGrid::OnGridSlotUnhovered(int32 GridIndex, const FPointerEvent& MouseEvent)
{
	UFP_GridSlot* GridSlot = GridSlots[GridIndex];
	if (GridSlot->IsAvailable())
	{
		GridSlot->SetUnoccupiedTexture();
	}
}
