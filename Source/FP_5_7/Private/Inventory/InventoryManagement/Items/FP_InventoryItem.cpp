// Copyright JG

#include "Inventory/InventoryManagement/Items/FP_InventoryItem.h"
#include "Net/UnrealNetwork.h"

void UFP_InventoryItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UFP_InventoryItem, ItemManifest);
}

void UFP_InventoryItem::SetItemManifest(const FFP_ItemManifest& Manifest)
{
	ItemManifest.InitializeAs<FFP_ItemManifest>(Manifest);
}
