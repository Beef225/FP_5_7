// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "Inventory/Items/Manifest/FP_ItemManifest.h"
#include "StructUtils/InstancedStruct.h"
#include "FP_InventoryItem.generated.h"

/**
 * Base class for all inventory items. Lives as a UObject inside the inventory's
 * FastArray; replicated to clients via FFP_InventoryFastArray.
 */
UCLASS(BlueprintType, Blueprintable)
class FP_5_7_API UFP_InventoryItem : public UObject
{
	GENERATED_BODY()

public:

	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;

	void SetItemManifest(const FFP_ItemManifest& Manifest);

private:

	UPROPERTY(Replicated, VisibleAnywhere, Category = "Inventory",
		meta = (BaseStruct = "/Script/FP_5_7.FP_ItemManifest"))
	FInstancedStruct ItemManifest;
};
