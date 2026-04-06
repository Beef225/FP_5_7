// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FP_InventoryGrid.generated.h"

class UCanvasPanel;
class UFP_GridSlot;

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

private:

	void ConstructGrid();

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCanvasPanel> CanvasPanel;

	UPROPERTY()
	TArray<TObjectPtr<UFP_GridSlot>> GridSlots;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TSubclassOf<UFP_GridSlot> GridSlotClass;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 Rows = 6;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 Columns = 8;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float TileSize = 64.f;
};
