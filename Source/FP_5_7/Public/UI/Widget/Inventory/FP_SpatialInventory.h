// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/Inventory/FP_InventoryBase.h"
#include "FP_SpatialInventory.generated.h"

class UFP_EquippedGridSlot;
class UFP_EquippedSlottedItem;
class UFP_InventoryGrid;
class UFP_ItemDescription;
class UCanvasPanel;

/**
 * Spatial (grid-based) inventory widget. Contains a single grid for all item categories.
 */
UCLASS()
class FP_5_7_API UFP_SpatialInventory : public UFP_InventoryBase
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FFP_SlotAvailabilityResult HasRoomForItem(UFP_ItemComponent* ItemComponent) const override;

	virtual void OnItemHovered(UFP_SlottedItem* SlottedItem) override;
	virtual void OnItemUnhovered(UFP_SlottedItem* SlottedItem) override;
	virtual bool HasHoverItem() const override;
	virtual UFP_HoverItem* GetHoverItem() const override;
	virtual void DropHoverItem() override;

	UFUNCTION()
	void EquippedGridSlotClicked(UFP_EquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentTypeTag);

	UFUNCTION()
	void EquippedSlottedItemClicked(UFP_EquippedSlottedItem* EquippedSlottedItem);

private:

	UFP_ItemDescription* GetItemDescription();
	void SetItemDescriptionSizeAndPosition();
	bool CanEquipHoverItem(UFP_EquippedGridSlot* EquippedGridSlot, const FGameplayTag& EquipmentTypeTag) const;
	UFP_EquippedGridSlot* FindSlotWithEquippedItem(UFP_InventoryItem* EquippedItem) const;
	void ClearSlotOfItem(UFP_EquippedGridSlot* EquippedGridSlot);
	void RemoveEquippedSlottedItem(UFP_EquippedSlottedItem* EquippedSlottedItem);
	void MakeEquippedSlottedItem(UFP_EquippedSlottedItem* EquippedSlottedItem, UFP_EquippedGridSlot* EquippedGridSlot, UFP_InventoryItem* ItemToEquip);
	void BroadcastSlotClickedDelegates(UFP_InventoryItem* ItemToEquip, UFP_InventoryItem* ItemToUnequip) const;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UFP_InventoryGrid> Grid;

	UPROPERTY()
	TArray<TObjectPtr<UFP_EquippedGridSlot>> EquippedGridSlots;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UFP_ItemDescription> ItemDescriptionClass;

	UPROPERTY()
	TObjectPtr<UFP_ItemDescription> ItemDescription;

	FTimerHandle ItemDescriptionTimerHandle;

	/** Delay (seconds) before the item description tooltip appears on hover. */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float ItemDescriptionDelay{ 0.5f };

	bool bWasLMBDown = false;
};
