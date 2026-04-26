// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FP_GridSlot.generated.h"

class UImage;
class UFP_InventoryItem;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FFP_GridSlotEvent, int32, GridIndex, const FPointerEvent&, MouseEvent);

UENUM(BlueprintType)
enum class EFP_GridSlotState : uint8
{
	Unoccupied,
	Occupied,
	Selected,
	GrayedOut
};

/**
 * A single tile in the inventory grid. Bound to an Image widget in Blueprint.
 * Maintains a visual state (Unoccupied/Occupied/Selected/GrayedOut) driven by
 * per-state FSlateBrush properties set in the Blueprint subclass.
 */
UCLASS()
class FP_5_7_API UFP_GridSlot : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeOnMouseEnter(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& MouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(const FGeometry& MyGeometry, const FPointerEvent& MouseEvent) override;

	FFP_GridSlotEvent GridSlotClicked;
	FFP_GridSlotEvent GridSlotHovered;
	FFP_GridSlotEvent GridSlotUnhovered;

	void SetTileIndex(int32 Index) { TileIndex = Index; }
	int32 GetTileIndex() const { return TileIndex; }

	EFP_GridSlotState GetGridSlotState() const { return GridSlotState; }

	TWeakObjectPtr<UFP_InventoryItem> GetInventoryItem() const { return InventoryItem; }
	void SetInventoryItem(UFP_InventoryItem* Item);

	int32 GetStackCount() const { return StackCount; }
	void SetStackCount(int32 Count) { StackCount = Count; }

	int32 GetUpperLeftIndex() const { return UpperLeftIndex; }
	void SetUpperLeftIndex(int32 Index) { UpperLeftIndex = Index; }

	bool IsAvailable() const { return bAvailable; }
	void SetAvailable(bool bIsAvailable) { bAvailable = bIsAvailable; }

	void SetOccupiedTexture();
	void SetUnoccupiedTexture();
	void SetSelectedTexture();
	void SetGrayedOutTexture();

private:

	int32 TileIndex = 0;
	int32 StackCount = 0;
	int32 UpperLeftIndex{ INDEX_NONE };
	TWeakObjectPtr<UFP_InventoryItem> InventoryItem;
	bool bAvailable{ true };

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GridSlot;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Unoccupied;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Occupied;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_Selected;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FSlateBrush Brush_GrayedOut;

	EFP_GridSlotState GridSlotState{ EFP_GridSlotState::Unoccupied };
};
