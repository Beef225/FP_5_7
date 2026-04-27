// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "FP_InventorySaveData.generated.h"

USTRUCT()
struct FFP_GridItemSaveRecord
{
	GENERATED_BODY()

	/** Upper-left grid index where this item was placed. */
	UPROPERTY()
	int32 OriginIndex = 0;

	/** Stack count displayed in this grid slot (0 for non-stackable). */
	UPROPERTY()
	int32 StackCount = 0;

	UPROPERTY(meta = (BaseStruct = "/Script/FP_5_7.FP_ItemManifest"))
	FInstancedStruct ItemManifest;
};

USTRUCT()
struct FFP_EquippedItemSaveRecord
{
	GENERATED_BODY()

	/** Equipment slot tag identifying which slot this item occupied. */
	UPROPERTY()
	FGameplayTag SlotTag;

	UPROPERTY(meta = (BaseStruct = "/Script/FP_5_7.FP_ItemManifest"))
	FInstancedStruct ItemManifest;
};

/**
 * Per-character inventory save: grid positions, equipped items, and hover item.
 * Saved to slot "FP_Inventory_<CharacterID>".
 */
UCLASS()
class FP_5_7_API UFP_InventorySaveData : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY()
	TArray<FFP_GridItemSaveRecord> GridItems;

	UPROPERTY()
	TArray<FFP_EquippedItemSaveRecord> EquippedItems;

	UPROPERTY()
	bool bHasHoverItem = false;

	/** Stack count on the hover item at save time. */
	UPROPERTY()
	int32 HoverItemStackCount = 0;

	UPROPERTY(meta = (BaseStruct = "/Script/FP_5_7.FP_ItemManifest"))
	FInstancedStruct HoverItemManifest;
};
