// Copyright JG

#include "UI/Widget/Inventory/FP_InventoryGrid.h"
#include "Blueprint/WidgetLayoutLibrary.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Inventory/InventoryManagement/Components/FP_InventoryComponent.h"
#include "Inventory/InventoryManagement/Items/FP_InventoryItem.h"
#include "Libraries/FP_AbilitySystemLibrary.h"
#include "Libraries/FP_WidgetUtils.h"
#include "UI/Widget/Inventory/GridSlots/FP_GridSlot.h"

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

void UFP_InventoryGrid::AddItem(UFP_InventoryItem* Item)
{
	if (!IsValid(Item)) return;

	UE_LOG(LogTemp, Log, TEXT("UFP_InventoryGrid::AddItem — received item type: %s"),
		*Item->GetItemManifest().GetItemType().ToString());
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
