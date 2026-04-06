// Copyright JG

#include "Inventory/InventoryManagement/Components/FP_InventoryComponent.h"
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

	const FFP_SlotAvailabilityResult Result = InventoryMenu->HasRoomForItem(ItemComponent);

	if (Result.TotalRoomToFill == 0)
	{
		NoRoomInInventory.Broadcast();
		return;
	}

	if (Result.Item.IsValid() && Result.bStackable)
	{
		// Item already exists in inventory — just add stacks, don't create a new entry.
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
	InventoryList.AddEntry(ItemComponent);
}

void UFP_InventoryComponent::Server_AddStacksToItem_Implementation(UFP_ItemComponent* ItemComponent, int32 StackCount, int32 Remainder)
{
	// TODO: Find existing item entry in FastArray, increment stack count.
}
