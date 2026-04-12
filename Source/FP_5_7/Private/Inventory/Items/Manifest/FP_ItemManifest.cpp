// Copyright JG

#include "Inventory/Items/Manifest/FP_ItemManifest.h"
#include "Inventory/InventoryManagement/Items/FP_InventoryItem.h"
#include "Inventory/Items/FP_ItemComponent.h"
#include "Inventory/Items/Fragments/FP_ItemFragment.h"
#include "UI/Widget/Inventory/Composite/FP_CompositeBase.h"

UFP_InventoryItem* FFP_ItemManifest::Manifest(UObject* NewOuter)
{
	UFP_InventoryItem* Item = NewObject<UFP_InventoryItem>(NewOuter, UFP_InventoryItem::StaticClass());
	Item->SetItemManifest(*this);

	// Give each fragment a chance to do pickup-time work on the inventory copy.
	for (TInstancedStruct<FFP_ItemFragment>& Fragment : Item->GetItemManifestMutable().GetFragmentsMutable())
	{
		if (FFP_ItemFragment* FragPtr = Fragment.GetMutablePtr<FFP_ItemFragment>())
		{
			FragPtr->Manifest();
		}
	}

	// Clear the source (world item) manifest — the inventory item now owns the data.
	ClearFragments();

	return Item;
}

void FFP_ItemManifest::ClearFragments()
{
	for (TInstancedStruct<FFP_ItemFragment>& Fragment : Fragments)
	{
		Fragment.Reset();
	}
	Fragments.Empty();
}

void FFP_ItemManifest::AssimilateInventoryFragments(UFP_CompositeBase* Composite) const
{
	const TArray<const FFP_InventoryItemFragment*> InventoryFragments = GetAllFragmentsOfType<FFP_InventoryItemFragment>();
	for (const FFP_InventoryItemFragment* Fragment : InventoryFragments)
	{
		Composite->ApplyFunction([Fragment](UFP_CompositeBase* Widget)
		{
			Fragment->Assimilate(Widget);
		});
	}
}

void FFP_ItemManifest::NotifyItemSpawned()
{
	for (TInstancedStruct<FFP_ItemFragment>& Fragment : Fragments)
	{
		if (FFP_ItemFragment* FragPtr = Fragment.GetMutablePtr<FFP_ItemFragment>())
		{
			FragPtr->OnSpawned();
		}
	}
}

void FFP_ItemManifest::SpawnPickupActor(const UObject* WorldContextObject, const FVector& SpawnLocation, const FRotator& SpawnRotation)
{
	if (!IsValid(PickupActorClass) || !IsValid(WorldContextObject)) return;

	// Deferred spawn so InitItemManifest runs before BeginPlay, guaranteeing that
	// fragments see the correct manifest data when OnSpawned() fires.
	const FTransform SpawnTransform(SpawnRotation, SpawnLocation);
	AActor* SpawnedActor = WorldContextObject->GetWorld()->SpawnActorDeferred<AActor>(
		PickupActorClass, SpawnTransform);
	if (!IsValid(SpawnedActor)) return;

	UFP_ItemComponent* ItemComp = SpawnedActor->FindComponentByClass<UFP_ItemComponent>();
	check(ItemComp);
	ItemComp->InitItemManifest(*this);

	SpawnedActor->FinishSpawning(SpawnTransform);
}
