// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "Inventory/InventoryManagement/Items/FP_InventoryItem.h"
#include "FP_SplitStackWidget.generated.h"

class UButton;
class USlider;
class UTextBlock;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FFP_SplitStackConfirmed, int32, SplitAmount, int32, GridIndex);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFP_SplitStackCancelled);

/**
 * Small widget shown on Ctrl+Right-click on a stackable item.
 * Lets the player choose how many stacks to split off via a slider,
 * then confirm or cancel.
 */
UCLASS()
class FP_5_7_API UFP_SplitStackWidget : public UUserWidget
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized() override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	/** Initialise the slider range and display for the given item and slot. */
	void SetupForItem(UFP_InventoryItem* Item, int32 StackCount, int32 InGridIndex);

	FFP_SplitStackConfirmed OnSplitConfirmed;
	FFP_SplitStackCancelled OnSplitCancelled;

private:

	UFUNCTION()
	void OnSliderValueChanged(float Value);

	UFUNCTION()
	void OnConfirmClicked();

	UFUNCTION()
	void OnCancelClicked();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USlider> Slider_Split;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_SplitAmount;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Confirm;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UButton> Button_Cancel;

	TWeakObjectPtr<UFP_InventoryItem> InventoryItem;
	int32 MaxStackCount{ 0 };
	int32 GridIndex{ INDEX_NONE };
};
