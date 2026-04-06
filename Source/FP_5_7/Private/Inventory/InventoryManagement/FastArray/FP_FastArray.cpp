// Copyright JG

#include "Inventory/InventoryManagement/FastArray/FP_FastArray.h"
#include "Inventory/InventoryManagement/Components/FP_InventoryComponent.h"
#include "Inventory/InventoryManagement/Items/FP_InventoryItem.h"
#include "Inventory/Items/FP_ItemComponent.h"

TArray<UFP_InventoryItem*> FFP_InventoryFastArray::GetAllItems() const
{
	TArray<UFP_InventoryItem*> Results;
	Results.Reserve(Entries.Num());
	for (const FFP_InventoryEntry& Entry : Entries)
	{
		if (!IsValid(Entry.Item)) continue;
		Results.Add(Entry.Item);
	}
	return Results;
}

void FFP_InventoryFastArray::PreReplicatedRemove(const TArrayView<int32> RemovedIndices, int32 FinalSize)
{
	UFP_InventoryComponent* IC = Cast<UFP_InventoryComponent>(OwnerComponent);
	if (!IsValid(IC)) return;

	for (int32 Index : RemovedIndices)
	{
		IC->OnItemRemoved.Broadcast(Entries[Index].Item);
	}
}

void FFP_InventoryFastArray::PostReplicatedAdd(const TArrayView<int32> AddedIndices, int32 FinalSize)
{
	UFP_InventoryComponent* IC = Cast<UFP_InventoryComponent>(OwnerComponent);
	if (!IsValid(IC)) return;

	for (int32 Index : AddedIndices)
	{
		IC->OnItemAdded.Broadcast(Entries[Index].Item);
	}
}

UFP_InventoryItem* FFP_InventoryFastArray::AddEntry(UFP_ItemComponent* ItemComponent)
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	UFP_InventoryComponent* IC = Cast<UFP_InventoryComponent>(OwnerComponent);
	check(IC);

	FFP_InventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Item = ItemComponent->GetItemManifest().Manifest(OwningActor);

	IC->AddRepSubObj(NewEntry.Item);
	MarkItemDirty(NewEntry);

	return NewEntry.Item;
}

UFP_InventoryItem* FFP_InventoryFastArray::AddEntry(UFP_InventoryItem* Item)
{
	check(OwnerComponent);
	AActor* OwningActor = OwnerComponent->GetOwner();
	check(OwningActor->HasAuthority());

	FFP_InventoryEntry& NewEntry = Entries.AddDefaulted_GetRef();
	NewEntry.Item = Item;
	MarkItemDirty(NewEntry);

	return Item;
}

void FFP_InventoryFastArray::RemoveEntry(UFP_InventoryItem* Item)
{
	for (auto EntryIt = Entries.CreateIterator(); EntryIt; ++EntryIt)
	{
		if (EntryIt->Item == Item)
		{
			EntryIt.RemoveCurrent();
			MarkArrayDirty();
			return;
		}
	}
}
