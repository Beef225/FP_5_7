// Copyright JG

#include "Inventory/InventoryManagement/Components/FP_InventoryComponent.h"
#include "Inventory/InventoryManagement/Items/FP_InventoryItem.h"
#include "Inventory/Items/Fragments/FP_ItemFragment.h"
#include "Inventory/Types/FP_InventoryTypes.h"
#include "Net/UnrealNetwork.h"
#include "UI/Widget/Inventory/FP_InventoryBase.h"

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
}

void UFP_InventoryComponent::ConstructInventory()
{
	OwningController = Cast<APlayerController>(GetOwner());
	checkf(OwningController.IsValid(), TEXT("FP_InventoryComponent must be owned by a PlayerController."));
	if (!OwningController->IsLocalController()) return;

	InventoryMenu = CreateWidget<UFP_InventoryBase>(OwningController.Get(), InventoryMenuClass);
	// Start hidden — player opens it explicitly
	InventoryMenu->SetVisibility(ESlateVisibility::Collapsed);
	InventoryMenu->AddToViewport();
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
		NoRoomInInventory.Broadcast();
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
