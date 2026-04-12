// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "UI/Widget/Inventory/SlottedItems/FP_SlottedItem.h"
#include "FP_EquippedSlottedItem.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FFP_EquippedSlottedItemClicked, class UFP_EquippedSlottedItem*, SlottedItem);

/**
 * Widget representing an item slotted into an equipment slot.
 * Carries the EquipmentTypeTag so the slot knows which category this item occupies.
 * Clicking broadcasts OnEquippedSlottedItemClicked for the parent to handle unequip.
 */
UCLASS()
class FP_5_7_API UFP_EquippedSlottedItem : public UFP_SlottedItem
{
	GENERATED_BODY()

public:

	virtual FReply NativeOnMouseButtonDown(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	void SetEquipmentTypeTag(const FGameplayTag& Tag) { EquipmentTypeTag = Tag; }
	FGameplayTag GetEquipmentTypeTag() const { return EquipmentTypeTag; }

	FFP_EquippedSlottedItemClicked OnEquippedSlottedItemClicked;

private:

	UPROPERTY()
	FGameplayTag EquipmentTypeTag;
};
