// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "Inventory/Types/FP_InventoryTypes.h"
#include "FP_InventoryGrid.generated.h"

enum class EFP_GridSlotState : uint8;

class UFP_HoverItem;
class UFP_SplitStackWidget;
class UCanvasPanel;
class UFP_GridSlot;
class UFP_InventoryItem;
class UFP_InventoryComponent;
class UFP_ItemComponent;
class UFP_SlottedItem;
struct FFP_ItemManifest;
struct FFP_GridFragment;
struct FFP_ImageFragment;

/**
 * The single inventory grid that holds all items regardless of category.
 * Procedurally constructs a Rows x Columns grid of UFP_GridSlot tiles on a CanvasPanel.
 */
UCLASS()
class FP_5_7_API UFP_InventoryGrid : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	FFP_SlotAvailabilityResult HasRoomForItem(const UFP_ItemComponent* ItemComponent);
	FFP_SlotAvailabilityResult HasRoomForItem(const UFP_InventoryItem* Item);

	UFUNCTION()
	void AddItem(UFP_InventoryItem* Item);

	UFUNCTION()
	void AddStacks(const FFP_SlotAvailabilityResult& Result);

	UFUNCTION()
	void OnSlottedItemClicked(int32 GridIndex, const FPointerEvent& MouseEvent);

	UFUNCTION()
	void OnGridSlotClicked(int32 GridIndex, const FPointerEvent& MouseEvent);

	UFUNCTION()
	void OnGridSlotHovered(int32 GridIndex, const FPointerEvent& MouseEvent);

	UFUNCTION()
	void OnGridSlotUnhovered(int32 GridIndex, const FPointerEvent& MouseEvent);

private:

	void ConstructGrid();
	FFP_SlotAvailabilityResult HasRoomForItem(const FFP_ItemManifest& Manifest);
	bool IsIndexClaimed(const TSet<int32>& CheckedIndices, int32 Index) const;
	bool IsInGridBounds(int32 StartIndex, const FIntPoint& ItemDimensions) const;
	bool HasRoomAtIndex(const UFP_GridSlot* GridSlot,
		const FIntPoint& Dimensions,
		const TSet<int32>& CheckedIndices,
		TSet<int32>& OutTentativelyClaimed,
		const FGameplayTag& ItemType,
		int32 MaxStackSize);
	bool CheckSlotConstraints(const UFP_GridSlot* GridSlot,
		const UFP_GridSlot* SubGridSlot,
		const TSet<int32>& CheckedIndices,
		TSet<int32>& OutTentativelyClaimed,
		const FGameplayTag& ItemType,
		int32 MaxStackSize) const;
	bool HasValidItem(const UFP_GridSlot* GridSlot) const;
	bool IsUpperLeftSlot(const UFP_GridSlot* GridSlot, const UFP_GridSlot* SubGridSlot) const;
	bool DoesItemTypeMatch(const UFP_InventoryItem* SubItem, const FGameplayTag& ItemType) const;
	int32 DetermineFillAmountForSlot(bool bStackable, int32 MaxStackSize, int32 AmountToFill, const UFP_GridSlot* GridSlot) const;
	int32 GetStackAmount(const UFP_GridSlot* GridSlot) const;
	bool IsRightClick(const FPointerEvent& MouseEvent) const;
	bool IsLeftClick(const FPointerEvent& MouseEvent) const;
	void PickUp(UFP_InventoryItem* ClickedInventoryItem, int32 GridIndex);
	void AssignHoverItem(UFP_InventoryItem* InventoryItem);
	void AssignHoverItem(UFP_InventoryItem* InventoryItem, int32 GridIndex, int32 PreviousGridIndex);
	void RemoveItemFromGrid(UFP_InventoryItem* InventoryItem, int32 GridIndex);
	FIntPoint GetItemDimensions(const FFP_ItemManifest& Manifest) const;

	void AddItemToIndices(const FFP_SlotAvailabilityResult& Result, UFP_InventoryItem* NewItem);
	void AddItemAtIndex(UFP_InventoryItem* Item, int32 Index, bool bStackable, int32 StackAmount);
	UFP_SlottedItem* CreateSlottedItem(UFP_InventoryItem* Item, bool bStackable, int32 StackAmount,
		const FFP_GridFragment* GridFragment, const FFP_ImageFragment* ImageFragment, int32 Index);
	void AddSlottedItemToCanvas(int32 Index, const FFP_GridFragment* GridFragment, UFP_SlottedItem* SlottedItem) const;
	void UpdateGridSlots(UFP_InventoryItem* NewItem, int32 Index, bool bStackableItem, int32 StackAmount);
	FVector2D GetDrawSize(const FFP_GridFragment* GridFragment) const;
	void UpdateTileParameters(const FVector2D& CanvasPosition, const FVector2D& MousePosition);
	void OnTileParametersUpdated(const FFP_TileParameters& Parameters);
	FIntPoint CalculateHoveredCoordinates(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const;
	EFP_TileQuadrant CalculateTileQuadrant(const FVector2D& CanvasPosition, const FVector2D& MousePosition) const;
	FIntPoint CalculateStartingCoordinate(const FIntPoint& Coordinate, const FIntPoint& Dimensions, EFP_TileQuadrant Quadrant) const;
	FFP_SpaceQueryResult CheckHoverPosition(const FIntPoint& Position, const FIntPoint& Dimensions);
	bool CursorExitedCanvas(const FVector2D& BoundaryPos, const FVector2D& BoundarySize, const FVector2D& Location);
	void HighlightSlots(int32 Index, const FIntPoint& Dimensions);
	void UnHighlightSlots(int32 Index, const FIntPoint& Dimensions);
	void ChangeHoverType(int32 Index, const FIntPoint& Dimensions, EFP_GridSlotState GridSlotState);
	void PutDownOnIndex(int32 Index);
	void ClearHoverItem();
	void CreateSplitStackWidget(int32 GridIndex);
	void TryConsumeItem(int32 GridIndex);

	UFUNCTION()
	void OnSplitStackConfirmed(int32 SplitAmount, int32 GridIndex);

	UFUNCTION()
	void OnSplitStackCancelled();
	bool IsSameStackable(const UFP_InventoryItem* ClickedInventoryItem) const;
	void SwapWithHoverItem(UFP_InventoryItem* ClickedInventoryItem, int32 GridIndex);
	bool ShouldSwapStackCounts(int32 RoomInClickedSlot, int32 HoveredStackCount, int32 MaxStackSize) const;
	void SwapStackCounts(int32 ClickedStackCount, int32 HoveredStackCount, int32 Index);
	bool ShouldConsumeHoverItemStacks(int32 HoveredStackCount, int32 RoomInClickedSlot) const;
	void ConsumeHoverItemStacks(int32 ClickedStackCount, int32 HoveredStackCount, int32 Index);
	bool ShouldFillInStack(int32 RoomInClickedSlot, int32 HoveredStackCount) const;
	void FillInStack(int32 FillAmount, int32 Remainder, int32 Index);
	void ShowCursor();
	void HideCursor();
	UUserWidget* GetVisibleCursorWidget();
	UUserWidget* GetHiddenCursorWidget();
	void SetSlottedItemImage(const UFP_SlottedItem* SlottedItem, const FFP_GridFragment* GridFragment,
		const FFP_ImageFragment* ImageFragment) const;

	TWeakObjectPtr<UFP_InventoryComponent> InventoryComponent;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;

	UPROPERTY()
	TArray<TObjectPtr<UFP_GridSlot>> GridSlots;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UFP_GridSlot> GridSlotClass;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UFP_SlottedItem> SlottedItemClass;

	UPROPERTY()
	TMap<int32, TObjectPtr<UFP_SlottedItem>> SlottedItems;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 Rows = 6;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 Columns = 8;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float TileSize = 64.f;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UFP_HoverItem> HoverItemClass;

	UPROPERTY()
	TObjectPtr<UFP_HoverItem> HoverItem;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UFP_SplitStackWidget> SplitStackWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FVector2D SplitWidgetOffset{ 0.f, 0.f };

	UPROPERTY()
	TObjectPtr<UFP_SplitStackWidget> SplitStackWidget;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UUserWidget> VisibleCursorWidgetClass;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UUserWidget> HiddenCursorWidgetClass;

	UPROPERTY()
	TObjectPtr<UUserWidget> VisibleCursorWidget;

	UPROPERTY()
	TObjectPtr<UUserWidget> HiddenCursorWidget;

	FFP_TileParameters TileParameters;
	FFP_TileParameters LastTileParameters;

	/** Index where an item would be placed if we click at a valid location. */
	int32 ItemDropIndex{ INDEX_NONE };
	FFP_SpaceQueryResult CurrentQueryResult;

	bool bMouseWithinCanvas{ false };
	bool bLastMouseWithinCanvas{ false };

	int32 LastHighlightedIndex{ INDEX_NONE };
	FIntPoint LastHighlightedDimensions{ 1, 1 };
};
