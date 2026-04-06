// Copyright JG

#include "Inventory/Items/Manifest/FP_ItemManifest.h"
#include "Inventory/InventoryManagement/Items/FP_InventoryItem.h"

UFP_InventoryItem* FFP_ItemManifest::Manifest(UObject* NewOuter)
{
	UFP_InventoryItem* Item = NewObject<UFP_InventoryItem>(NewOuter, UFP_InventoryItem::StaticClass());
	Item->SetItemManifest(*this);
	return Item;
}
