// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "FP_GridSlot.h"
#include "GameplayTagContainer.h"
#include "UI/Widget/Inventory/SlottedItems/FP_EquippedSlottedItem.h"
#include "FP_EquippedGridSlot.generated.h"

class UImage;
class UOverlay;
class UFP_EquippedGridSlot;
class UFP_InventoryItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FFP_EquippedGridSlotClicked, UFP_EquippedGridSlot*, GridSlot, const FGameplayTag&, EquipmentTypeTag);

/**
 * A specialised grid slot for equipment panels.
 * Each slot carries an EquipmentTypeTag identifying what equipment category it accepts
 * (e.g. Equip.Weapon, Equip.Helmet). Clicking broadcasts the slot + tag to the parent
 * SpatialInventory so it can route the item into the correct equipment socket.
 */
UCLASS()
class FP_5_7_API UFP_EquippedGridSlot : public UFP_GridSlot
{
	GENERATED_BODY()

public:

	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	UFP_EquippedSlottedItem* OnItemEquipped(UFP_InventoryItem* Item, const FGameplayTag& EquipmentTag, float TileSize);
	void SetEquippedSlottedItem(UFP_EquippedSlottedItem* Item) { EquippedSlottedItem = Item; }

	FFP_EquippedGridSlotClicked EquippedGridSlotClicked;

private:

	/** Identifies what equipment category this slot accepts (set per-slot in the Blueprint Details panel). */
	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (Categories = "Inventory"))
	FGameplayTag EquipmentTypeTag;

	/** Grayed-out silhouette icon shown when no matching hover item is held over this slot. */
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GrayedOutIcon;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UFP_EquippedSlottedItem> EquippedSlottedItemClass;

	UPROPERTY()
	TObjectPtr<UFP_EquippedSlottedItem> EquippedSlottedItem;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UOverlay> Overlay_Root;
};
