// Copyright JG

#include "UI/WidgetController/FP_InventoryWidgetController.h"
#include "Inventory/InventoryManagement/Components/FP_InventoryComponent.h"

UFP_InventoryComponent* UFP_InventoryWidgetController::GetInventoryComponent() const
{
	if (!PlayerController) return nullptr;
	return PlayerController->FindComponentByClass<UFP_InventoryComponent>();
}

void UFP_InventoryWidgetController::BindCallbacksToDependencies()
{
	if (UFP_InventoryComponent* Comp = GetInventoryComponent())
	{
		Comp->NoRoomInInventory.AddDynamic(this, &UFP_InventoryWidgetController::OnNoRoom);
	}
}

void UFP_InventoryWidgetController::OnNoRoom()
{
	OnNoRoomInInventory.Broadcast();
}

void UFP_InventoryWidgetController::ToggleInventory()
{
	if (UFP_InventoryComponent* Comp = GetInventoryComponent())
	{
		Comp->ToggleInventory();
	}
}

void UFP_InventoryWidgetController::ShowInventory()
{
	if (UFP_InventoryComponent* Comp = GetInventoryComponent())
	{
		Comp->ShowInventory();
	}
}

void UFP_InventoryWidgetController::HideInventory()
{
	if (UFP_InventoryComponent* Comp = GetInventoryComponent())
	{
		Comp->HideInventory();
	}
}

bool UFP_InventoryWidgetController::IsInventoryVisible() const
{
	if (const UFP_InventoryComponent* Comp = GetInventoryComponent())
	{
		return Comp->IsInventoryVisible();
	}
	return false;
}
