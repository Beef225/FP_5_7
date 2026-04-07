// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/Types/FP_InventoryTypes.h"
#include "FP_InventoryGrid.generated.h"

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

	FFP_SlotAvailabilityResult HasRoomForItem(const UFP_ItemComponent* ItemComponent);
	FFP_SlotAvailabilityResult HasRoomForItem(const UFP_InventoryItem* Item);

	UFUNCTION()
	void AddItem(UFP_InventoryItem* Item);

private:

	void ConstructGrid();
	FFP_SlotAvailabilityResult HasRoomForItem(const FFP_ItemManifest& Manifest);

	void AddItemToIndices(const FFP_SlotAvailabilityResult& Result, UFP_InventoryItem* NewItem);
	void AddItemAtIndex(UFP_InventoryItem* Item, int32 Index, bool bStackable, int32 StackAmount);
	UFP_SlottedItem* CreateSlottedItem(UFP_InventoryItem* Item, bool bStackable, int32 StackAmount,
		const FFP_GridFragment* GridFragment, const FFP_ImageFragment* ImageFragment, int32 Index);
	void AddSlottedItemToCanvas(int32 Index, const FFP_GridFragment* GridFragment, UFP_SlottedItem* SlottedItem) const;
	void UpdateGridSlots(UFP_InventoryItem* NewItem, int32 Index, bool bStackableItem, int32 StackAmount);
	FVector2D GetDrawSize(const FFP_GridFragment* GridFragment) const;
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
};
