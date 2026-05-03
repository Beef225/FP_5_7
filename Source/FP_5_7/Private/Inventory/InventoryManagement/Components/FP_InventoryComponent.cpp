// Copyright JG

#include "Inventory/InventoryManagement/Components/FP_InventoryComponent.h"
#include "Inventory/InventoryManagement/Items/FP_InventoryItem.h"
#include "Inventory/Items/Fragments/FP_ItemFragment.h"
#include "Inventory/Items/Fragments/FP_AffixFragment.h"
#include "Inventory/Items/Fragments/FP_ImplicitFragment.h"
#include "Inventory/Items/Fragments/FP_SkillFragment.h"
#include "Inventory/Types/FP_InventoryTypes.h"
#include "Net/UnrealNetwork.h"
#include "SaveSystem/FP_InventorySaveData.h"
#include "SaveSystem/FP_SaveGameSubsystem.h"
#include "UI/Widget/Inventory/FP_InventoryBase.h"
#include "UI/Widget/Inventory/FP_SpatialInventory.h"
#include "UI/Widget/Inventory/FP_InventoryGrid.h"
#include "UI/Widget/Inventory/GridSlots/FP_EquippedGridSlot.h"
#include "UI/Widget/Inventory/SlottedItems/FP_SlottedItem.h"
#include "UI/Widget/Inventory/HoverItem/FP_HoverItem.h"

UFP_InventoryComponent::UFP_InventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
	SetIsReplicatedByDefault(true);
	bReplicateUsingRegisteredSubObjectList = true;
	InventoryList = FFP_InventoryFastArray(this);
}

void UFP_InventoryComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UFP_InventoryComponent, InventoryList);
}

void UFP_InventoryComponent::AddRepSubObj(UObject* SubObj)
{
	if (IsUsingRegisteredSubObjectList() && IsReadyForReplication() && IsValid(SubObj))
	{
		AddReplicatedSubObject(SubObj);
	}
}

void UFP_InventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	ConstructInventory();

	if (UFP_SaveGameSubsystem* SaveSys = UFP_SaveGameSubsystem::Get(this))
	{
		const FGuid CharacterID = SaveSys->GetProfile()
			? SaveSys->GetProfile()->LastPlayedCharacterID
			: FGuid();

		SaveSys->LoadInventory(this, CharacterID);
	}
}

void UFP_InventoryComponent::ConstructInventory()
{
	OwningController = Cast<APlayerController>(GetOwner());
	checkf(OwningController.IsValid(), TEXT("FP_InventoryComponent must be owned by a PlayerController."));
	if (!OwningController->IsLocalController()) return;

	InventoryMenu = CreateWidget<UFP_InventoryBase>(OwningController.Get(), InventoryMenuClass);
}

void UFP_InventoryComponent::ShowInventory()
{
	if (InventoryMenu)
	{
		InventoryMenu->SetVisibility(ESlateVisibility::Visible);
	}
}

void UFP_InventoryComponent::HideInventory()
{
	if (InventoryMenu)
	{
		InventoryMenu->SetVisibility(ESlateVisibility::Collapsed);
	}
}

void UFP_InventoryComponent::ToggleInventory()
{
	if (IsInventoryVisible())
	{
		HideInventory();
	}
	else
	{
		ShowInventory();
	}
	OnInventoryMenuToggled.Broadcast(IsInventoryVisible());
}

bool UFP_InventoryComponent::IsInventoryVisible() const
{
	return InventoryMenu && InventoryMenu->GetVisibility() == ESlateVisibility::Visible;
}

void UFP_InventoryComponent::TryAddItem(UFP_ItemComponent* ItemComponent)
{
	if (!InventoryMenu) return;

	FFP_SlotAvailabilityResult Result = InventoryMenu->HasRoomForItem(ItemComponent);
	Result.Item = InventoryList.FindFirstItemByType(ItemComponent->GetItemManifest().GetItemType());

	if (Result.TotalRoomToFill == 0)
	{
		// Copy the manifest before destroying the source actor
		FFP_ItemManifest ManifestCopy = ItemComponent->GetItemManifest();

		ItemComponent->GetOwner()->Destroy();

		if (const APawn* OwningPawn = OwningController->GetPawn())
		{
			FVector RotatedForward = OwningPawn->GetActorForwardVector();
			RotatedForward = RotatedForward.RotateAngleAxis(
				FMath::FRandRange(DropSpawnAngleMin, DropSpawnAngleMax), FVector::UpVector);
			FVector SpawnLocation = OwningPawn->GetActorLocation()
				+ RotatedForward * FMath::FRandRange(DropSpawnDistanceMin, DropSpawnDistanceMax);
			SpawnLocation.Z -= RelativeSpawnElevation;

			ManifestCopy.SpawnPickupActor(this, SpawnLocation, FRotator::ZeroRotator);
		}
		return;
	}

	if (Result.Item.IsValid() && Result.bStackable)
	{
		// Item already exists in inventory — just add stacks, don't create a new entry.
		OnStackChange.Broadcast(Result);
		Server_AddStacksToItem(ItemComponent, Result.TotalRoomToFill, Result.Remainder);
	}
	else if (Result.TotalRoomToFill > 0)
	{
		// New item type — create a fresh inventory entry and fill the relevant slots.
		Server_AddNewItem(ItemComponent, Result.bStackable ? Result.TotalRoomToFill : 0);
	}
}

void UFP_InventoryComponent::Server_AddNewItem_Implementation(UFP_ItemComponent* ItemComponent, int32 StackCount)
{
	UFP_InventoryItem* NewItem = InventoryList.AddEntry(ItemComponent);

	// On listen-server / standalone the PostReplicatedAdd callback never fires for the server's own copy,
	// so we broadcast the delegate manually here.
	const ENetMode NetMode = GetOwner()->GetNetMode();
	if (NetMode == NM_ListenServer || NetMode == NM_Standalone)
	{
		OnItemAdded.Broadcast(NewItem);
	}

	ItemComponent->PickedUp();
}

void UFP_InventoryComponent::Server_AddStacksToItem_Implementation(UFP_ItemComponent* ItemComponent, int32 StackCount, int32 Remainder)
{
	UFP_InventoryItem* Item = InventoryList.FindFirstItemByType(ItemComponent->GetItemManifest().GetItemType());
	if (!IsValid(Item)) return;

	Item->SetTotalStackCount(Item->GetTotalStackCount() + StackCount);

	if (Remainder == 0)
	{
		ItemComponent->PickedUp();
	}
	else if (FFP_StackableFragment* StackableFragment = ItemComponent->GetItemManifestMutable().GetFragmentOfTypeMutable<FFP_StackableFragment>())
	{
		StackableFragment->SetStackCount(Remainder);
	}
}

void UFP_InventoryComponent::Server_ConsumeItem_Implementation(UFP_InventoryItem* Item)
{
	const FString ItemName = Item->GetItemManifest().GetItemType().ToString();

	const int32 NewStackCount = Item->GetTotalStackCount() - 1;
	if (NewStackCount <= 0)
	{
		InventoryList.RemoveEntry(Item);
	}
	else
	{
		Item->SetTotalStackCount(NewStackCount);
	}

	GEngine->AddOnScreenDebugMessage(-1, 3.f, FColor::Green,
		FString::Printf(TEXT("%s consumed!"), *ItemName));

	if (FFP_ConsumableFragment* ConsumableFragment = Item->GetItemManifestMutable().GetFragmentOfTypeMutable<FFP_ConsumableFragment>())
	{
		ConsumableFragment->OnConsume(OwningController.Get());
	}
}

void UFP_InventoryComponent::Server_EquipSlotClicked_Implementation(UFP_InventoryItem* ItemToEquip, UFP_InventoryItem* ItemToUnequip)
{
	Multicast_EquipSlotClicked(ItemToEquip, ItemToUnequip);
}

void UFP_InventoryComponent::Multicast_EquipSlotClicked_Implementation(UFP_InventoryItem* ItemToEquip, UFP_InventoryItem* ItemToUnequip)
{
	APlayerController* PC = OwningController.Get();

	// Guard before touching anything — if requirements fail, both items stay in their current state
	if (IsValid(ItemToEquip))
	{
		if (const FFP_AttributeRequirementFragment* ReqFrag = ItemToEquip->GetItemManifest().GetFragmentOfType<FFP_AttributeRequirementFragment>())
		{
			if (!ReqFrag->MeetsRequirements(PC))
			{
				UE_LOG(LogTemp, Warning, TEXT("Cannot equip %s: attribute requirements not met."),
					*ItemToEquip->GetName());
				return;
			}
		}
	}

	if (IsValid(ItemToUnequip))
	{
		if (FFP_MeshFragment* MeshFrag = ItemToUnequip->GetItemManifestMutable().GetFragmentOfTypeMutable<FFP_MeshFragment>())
		{
			MeshFrag->OnUnequip(PC);
		}
		if (FFP_EquipmentFragment* EquipFrag = ItemToUnequip->GetItemManifestMutable().GetFragmentOfTypeMutable<FFP_EquipmentFragment>())
		{
			EquipFrag->OnUnequip(PC);
		}
		if (FFP_AffixFragment* AffixFrag = ItemToUnequip->GetItemManifestMutable().GetFragmentOfTypeMutable<FFP_AffixFragment>())
		{
			AffixFrag->OnUnequip(PC);
		}
		if (FFP_ImplicitFragment* ImplicitFrag = ItemToUnequip->GetItemManifestMutable().GetFragmentOfTypeMutable<FFP_ImplicitFragment>())
		{
			ImplicitFrag->OnUnequip(PC);
		}
		if (FFP_SkillFragment* SkillFrag = ItemToUnequip->GetItemManifestMutable().GetFragmentOfTypeMutable<FFP_SkillFragment>())
		{
			SkillFrag->OnUnequip(PC);
		}
	}

	if (IsValid(ItemToEquip))
	{
		if (FFP_MeshFragment* MeshFrag = ItemToEquip->GetItemManifestMutable().GetFragmentOfTypeMutable<FFP_MeshFragment>())
		{
			MeshFrag->OnEquip(PC);
		}
		if (FFP_EquipmentFragment* EquipFrag = ItemToEquip->GetItemManifestMutable().GetFragmentOfTypeMutable<FFP_EquipmentFragment>())
		{
			EquipFrag->OnEquip(PC);
		}
		if (FFP_AffixFragment* AffixFrag = ItemToEquip->GetItemManifestMutable().GetFragmentOfTypeMutable<FFP_AffixFragment>())
		{
			AffixFrag->OnEquip(PC);
		}
		if (FFP_ImplicitFragment* ImplicitFrag = ItemToEquip->GetItemManifestMutable().GetFragmentOfTypeMutable<FFP_ImplicitFragment>())
		{
			ImplicitFrag->OnEquip(PC);
		}
		if (FFP_SkillFragment* SkillFrag = ItemToEquip->GetItemManifestMutable().GetFragmentOfTypeMutable<FFP_SkillFragment>())
		{
			SkillFrag->OnEquip(PC);
		}
	}

	OnItemEquipped.Broadcast(ItemToEquip);
	OnItemUnequipped.Broadcast(ItemToUnequip);
}

void UFP_InventoryComponent::Server_DropItem_Implementation(UFP_InventoryItem* Item, int32 StackCount)
{
	const int32 NewStackCount = Item->GetTotalStackCount() - StackCount;
	if (NewStackCount <= 0)
	{
		InventoryList.RemoveEntry(Item);
	}
	else
	{
		Item->SetTotalStackCount(NewStackCount);
	}

	SpawnDroppedItem(Item, StackCount);
}

void UFP_InventoryComponent::SpawnDroppedItem(UFP_InventoryItem* Item, int32 StackCount)
{
	const APawn* OwningPawn = OwningController->GetPawn();
	FVector RotatedForward = OwningPawn->GetActorForwardVector();
	RotatedForward = RotatedForward.RotateAngleAxis(
		FMath::FRandRange(DropSpawnAngleMin, DropSpawnAngleMax), FVector::UpVector);
	FVector SpawnLocation = OwningPawn->GetActorLocation()
		+ RotatedForward * FMath::FRandRange(DropSpawnDistanceMin, DropSpawnDistanceMax);
	SpawnLocation.Z -= RelativeSpawnElevation;
	const FRotator SpawnRotation = FRotator::ZeroRotator;

	FFP_ItemManifest& ItemManifest = Item->GetItemManifestMutable();
	if (FFP_StackableFragment* StackableFragment = ItemManifest.GetFragmentOfTypeMutable<FFP_StackableFragment>())
	{
		StackableFragment->SetStackCount(StackCount);
	}
	ItemManifest.SpawnPickupActor(this, SpawnLocation, SpawnRotation);
}

UFP_InventorySaveData* UFP_InventoryComponent::CaptureInventoryState() const
{
	UFP_SpatialInventory* Spatial = Cast<UFP_SpatialInventory>(InventoryMenu);
	if (!IsValid(Spatial)) return nullptr;

	UFP_InventoryGrid* Grid = Spatial->GetGrid();
	if (!IsValid(Grid)) return nullptr;

	UFP_InventorySaveData* SaveData = NewObject<UFP_InventorySaveData>(GetTransientPackage());

	// --- Grid items ---
	TSet<UFP_InventoryItem*> SavedItems;

	for (const auto& [OriginIndex, SlottedItem] : Grid->GetSlottedItems())
	{
		UFP_InventoryItem* Item = IsValid(SlottedItem) ? SlottedItem->GetInventoryItem() : nullptr;
		if (!IsValid(Item)) continue;

		FFP_GridItemSaveRecord& Record = SaveData->GridItems.AddDefaulted_GetRef();
		Record.OriginIndex = OriginIndex;
		Record.StackCount  = Item->GetTotalStackCount();
		Record.ItemManifest.InitializeAs<FFP_ItemManifest>(Item->GetItemManifest());

		SavedItems.Add(Item);
	}

	// --- Equipped items ---
	for (const UFP_EquippedGridSlot* Slot : Spatial->GetEquippedGridSlots())
	{
		if (!IsValid(Slot)) continue;

		const UFP_EquippedSlottedItem* SlottedItem = Slot->GetEquippedSlottedItem();
		if (!IsValid(SlottedItem)) continue;

		UFP_InventoryItem* Item = SlottedItem->GetInventoryItem();
		if (!IsValid(Item)) continue;

		FFP_EquippedItemSaveRecord& Record = SaveData->EquippedItems.AddDefaulted_GetRef();
		Record.SlotTag     = Slot->GetEquipmentTypeTag();
		Record.ItemManifest.InitializeAs<FFP_ItemManifest>(Item->GetItemManifest());

		SavedItems.Add(Item);
	}

	// --- Hover item (only if its backing item is not already captured above) ---
	const UFP_HoverItem* HoverItem = Grid->GetHoverItem();
	if (IsValid(HoverItem))
	{
		UFP_InventoryItem* HoverInventoryItem = HoverItem->GetInventoryItem();
		if (IsValid(HoverInventoryItem) && !SavedItems.Contains(HoverInventoryItem))
		{
			SaveData->bHasHoverItem       = true;
			SaveData->HoverItemStackCount = HoverItem->GetStackCount();
			SaveData->HoverItemManifest.InitializeAs<FFP_ItemManifest>(HoverInventoryItem->GetItemManifest());
		}
	}

	return SaveData;
}

void UFP_InventoryComponent::RestoreInventoryState(UFP_InventorySaveData* SaveData)
{
	if (!IsValid(SaveData)) return;
	if (!OwningController.IsValid() || !OwningController->IsLocalController()) return;

	UFP_SpatialInventory* Spatial = Cast<UFP_SpatialInventory>(InventoryMenu);
	if (!IsValid(Spatial)) return;

	UFP_InventoryGrid* Grid = Spatial->GetGrid();
	if (!IsValid(Grid)) return;

	APlayerController* PC = OwningController.Get();
	AActor* OwningActor = GetOwner();

	auto CreateItem = [&](const FInstancedStruct& ManifestStruct, int32 StackCount) -> UFP_InventoryItem*
	{
		const FFP_ItemManifest* Manifest = ManifestStruct.GetPtr<FFP_ItemManifest>();
		if (!Manifest) return nullptr;

		UFP_InventoryItem* Item = NewObject<UFP_InventoryItem>(OwningActor);
		Item->SetItemManifest(*Manifest);
		Item->SetTotalStackCount(StackCount);
		AddRepSubObj(Item);
		InventoryList.AddEntry(Item);
		return Item;
	};

	// --- Restore grid items ---
	for (const FFP_GridItemSaveRecord& Record : SaveData->GridItems)
	{
		UFP_InventoryItem* Item = CreateItem(Record.ItemManifest, Record.StackCount);
		if (!Item) continue;
		Grid->RestoreItemAtIndex(Item, Record.OriginIndex);
	}

	// --- Restore equipped items (bypasses MeetsRequirements — items were validly worn at save) ---
	for (const FFP_EquippedItemSaveRecord& Record : SaveData->EquippedItems)
	{
		UFP_InventoryItem* Item = CreateItem(Record.ItemManifest, 0);
		if (!Item) continue;

		// Restore visual in the equipment slot
		Spatial->RestoreEquippedItem(Item, Record.SlotTag);

		// Reapply all equipment GEs / effects — no requirements check on restore
		if (FFP_MeshFragment* MeshFrag = Item->GetItemManifestMutable().GetFragmentOfTypeMutable<FFP_MeshFragment>())
			MeshFrag->OnEquip(PC);
		if (FFP_EquipmentFragment* EquipFrag = Item->GetItemManifestMutable().GetFragmentOfTypeMutable<FFP_EquipmentFragment>())
			EquipFrag->OnEquip(PC);
		if (FFP_AffixFragment* AffixFrag = Item->GetItemManifestMutable().GetFragmentOfTypeMutable<FFP_AffixFragment>())
			AffixFrag->OnEquip(PC);
		if (FFP_ImplicitFragment* ImplicitFrag = Item->GetItemManifestMutable().GetFragmentOfTypeMutable<FFP_ImplicitFragment>())
			ImplicitFrag->OnEquip(PC);
		if (FFP_SkillFragment* SkillFrag = Item->GetItemManifestMutable().GetFragmentOfTypeMutable<FFP_SkillFragment>())
			SkillFrag->OnEquip(PC);
	}

	// --- Restore hover item ---
	if (SaveData->bHasHoverItem)
	{
		UFP_InventoryItem* Item = CreateItem(SaveData->HoverItemManifest, SaveData->HoverItemStackCount);
		if (Item)
		{
			Grid->AssignHoverItem(Item);
			if (UFP_HoverItem* RestoredHover = Grid->GetHoverItem())
			{
				RestoredHover->UpdateStackCount(Item->IsStackable() ? SaveData->HoverItemStackCount : 0);
			}
		}
	}
}
