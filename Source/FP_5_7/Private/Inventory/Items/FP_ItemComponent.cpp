// Copyright JG

#include "Inventory/Items/FP_ItemComponent.h"
#include "Net/UnrealNetwork.h"

void UFP_ItemComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	DOREPLIFETIME(UFP_ItemComponent, ItemManifest);
}
