// Copyright JG

#include "UI/Widget/Inventory/GridSlots/FP_GridSlot.h"
#include "Components/Image.h"
#include "Inventory/InventoryManagement/Items/FP_InventoryItem.h"

void UFP_GridSlot::SetInventoryItem(UFP_InventoryItem* Item)
{
	InventoryItem = Item;
}

void UFP_GridSlot::SetOccupiedTexture()
{
	GridSlotState = EFP_GridSlotState::Occupied;
	Image_GridSlot->SetBrush(Brush_Occupied);
}

void UFP_GridSlot::SetUnoccupiedTexture()
{
	GridSlotState = EFP_GridSlotState::Unoccupied;
	Image_GridSlot->SetBrush(Brush_Unoccupied);
}

void UFP_GridSlot::SetSelectedTexture()
{
	GridSlotState = EFP_GridSlotState::Selected;
	Image_GridSlot->SetBrush(Brush_Selected);
}

void UFP_GridSlot::SetGrayedOutTexture()
{
	GridSlotState = EFP_GridSlotState::GrayedOut;
	Image_GridSlot->SetBrush(Brush_GrayedOut);
}
