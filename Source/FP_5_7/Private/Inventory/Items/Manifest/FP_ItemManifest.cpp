// Copyright JG

#include "Inventory/Items/Manifest/FP_ItemManifest.h"
#include "Inventory/InventoryManagement/Items/FP_InventoryItem.h"
#include "Inventory/Items/FP_ItemComponent.h"

UFP_InventoryItem* FFP_ItemManifest::Manifest(UObject* NewOuter)
{
	UFP_InventoryItem* Item = NewObject<UFP_InventoryItem>(NewOuter, UFP_InventoryItem::StaticClass());
	Item->SetItemManifest(*this);
	return Item;
}

void FFP_ItemManifest::SpawnPickupActor(const UObject* WorldContextObject, const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	if (!IsValid(PickupActorClass) || !IsValid(WorldContextObject)) return;

	AActor* SpawnedActor = WorldContextObject->GetWorld()->SpawnActor<AActor>(PickupActorClass, SpawnLocation, SpawnRotation);
	if (!IsValid(SpawnedActor)) return;

	UFP_ItemComponent* ItemComp = SpawnedActor->FindComponentByClass<UFP_ItemComponent>();
	check(ItemComp);

	ItemComp->InitItemManifest(*this);
}
