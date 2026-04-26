// Copyright JG

#include "Inventory/Items/FP_ItemComponent.h"
#include "Net/UnrealNetwork.h"

UFP_ItemComponent::UFP_ItemComponent()
{
	SetIsReplicatedByDefault(true);
}

void UFP_ItemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UFP_ItemComponent, ItemManifest);
}

void UFP_ItemComponent::PickedUp()
{
	OnPickedUp();
	GetOwner()->Destroy();
}

void UFP_ItemComponent::InitItemManifest(FFP_ItemManifest CopyOfManifest)
{
	ItemManifest = CopyOfManifest;
}
