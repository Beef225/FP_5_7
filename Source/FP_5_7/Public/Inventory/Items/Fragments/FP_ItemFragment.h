// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "FP_ItemFragment.generated.h"

class APlayerController;

/**
 * Base struct for all item fragments.
 * Fragments are composable data blobs stored on FFP_ItemManifest via TInstancedStruct.
 * Each concrete fragment is identified by its FragmentTag (e.g. Fragment.Grid).
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_ItemFragment
{
	GENERATED_BODY()

	FFP_ItemFragment() {}
	FFP_ItemFragment(const FFP_ItemFragment&) = default;
	FFP_ItemFragment& operator=(const FFP_ItemFragment&) = default;
	FFP_ItemFragment(FFP_ItemFragment&&) = default;
	FFP_ItemFragment& operator=(FFP_ItemFragment&&) = default;
	virtual ~FFP_ItemFragment() {}

	FGameplayTag GetFragmentTag() const { return FragmentTag; }
	void SetFragmentTag(FGameplayTag Tag) { FragmentTag = Tag; }

private:

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (Categories = "Fragment"))
	FGameplayTag FragmentTag = FGameplayTag::EmptyTag;
};

/**
 * Defines the item's footprint on the inventory grid.
 * GridSize: how many tiles the item occupies (width x height).
 * GridPadding: visual padding inside the tile border.
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_GridFragment : public FFP_ItemFragment
{
	GENERATED_BODY()

	FIntPoint GetGridSize() const { return GridSize; }
	void SetGridSize(const FIntPoint& Size) { GridSize = Size; }

	float GetGridPadding() const { return GridPadding; }
	void SetGridPadding(float Padding) { GridPadding = Padding; }

private:

	/** Tile footprint of this item on the inventory grid (columns x rows). */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	FIntPoint GridSize{ 1, 1 };

	/** Visual padding inside the grid slot border. */
	UPROPERTY(EditAnywhere, Category = "Inventory")
	float GridPadding{ 0.f };
};

/**
 * Stores the item's icon texture and the 2D dimensions at which it should be displayed.
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_ImageFragment : public FFP_ItemFragment
{
	GENERATED_BODY()

	UTexture2D* GetIcon() const { return Icon; }

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TObjectPtr<UTexture2D> Icon{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FVector2D IconDimensions{ 44.f, 44.f };
};

/**
 * Marks an item as stackable. Defines how many instances can share one inventory slot
 * and tracks the current stack count.
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_StackableFragment : public FFP_ItemFragment
{
	GENERATED_BODY()

	int32 GetMaxStackSize() const { return MaxStackSize; }
	int32 GetStackCount() const { return StackCount; }
	void SetStackCount(int32 Count) { StackCount = Count; }

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MaxStackSize{ 1 };

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 StackCount{ 1 };
};

/**
 * Marker fragment for consumable items.
 * OnConsume is a virtual hook for future GAS/stats integration — currently unused.
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_ConsumableFragment : public FFP_ItemFragment
{
	GENERATED_BODY()

	virtual void OnConsume(APlayerController* PC) {}
};
