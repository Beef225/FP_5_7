// Copyright JG

#include "Inventory/InventoryManagement/Items/FP_InventoryItem.h"
#include "Inventory/Items/Fragments/FP_ItemFragment.h"
#include "Libraries/FP_EnumDefs.h"
#include "Net/UnrealNetwork.h"

void UFP_InventoryItem::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	UObject::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UFP_InventoryItem, TotalStackCount);
	DOREPLIFETIME(UFP_InventoryItem, ItemManifest);
}

void UFP_InventoryItem::SetItemManifest(const FFP_ItemManifest& Manifest)
{
	ItemManifest.InitializeAs<FFP_ItemManifest>(Manifest);
}

bool UFP_InventoryItem::IsStackable() const
{
	const FFP_StackableFragment* Stackable = GetItemManifest().GetFragmentOfType<FFP_StackableFragment>();
	return Stackable != nullptr;
}

bool UFP_InventoryItem::IsConsumable() const
{
	return GetItemManifest().GetItemCategory() == EItemCategory::Consumable;
}
