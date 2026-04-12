// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "FP_ItemFragment.generated.h"

class APlayerController;
class UFP_CompositeBase;

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

	/**
	 * Called on each fragment when FFP_ItemManifest::Manifest() creates the inventory item.
	 * Override to perform any one-time per-fragment work at pickup time on the inventory copy.
	 */
	virtual void Manifest() {}

	/**
	 * Called once on every fragment when the item actor spawns in the world
	 * (both for editor-placed and code-spawned actors). Override to roll random
	 * values or do any one-time initialisation at spawn time.
	 */
	virtual void OnSpawned() {}

private:

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (Categories = "Fragment"))
	FGameplayTag FragmentTag = FGameplayTag::EmptyTag;
};

/**
 * Intermediate base for fragments that have a corresponding composite leaf widget
 * in the item description UI. Assimilate() expands matching leaf widgets based on
 * the fragment's tag.
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_InventoryItemFragment : public FFP_ItemFragment
{
	GENERATED_BODY()

	/** Expands the composite widget if its FragmentTag matches this fragment's tag. */
	virtual void Assimilate(UFP_CompositeBase* Composite) const;

protected:

	bool MatchesWidgetTag(const UFP_CompositeBase* Composite) const;
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
 * Inherits from FFP_InventoryItemFragment so it can populate UFP_Leaf_Image widgets
 * in the description composite tree.
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_ImageFragment : public FFP_InventoryItemFragment
{
	GENERATED_BODY()

	UTexture2D* GetIcon() const { return Icon; }
	virtual void Assimilate(UFP_CompositeBase* Composite) const override;

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	TObjectPtr<UTexture2D> Icon{ nullptr };

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FVector2D IconDimensions{ 44.f, 44.f };
};

/**
 * Carries a text string for display in the item description UI (e.g. item name).
 * Each leaf widget tagged Fragment.ItemName in the description composite will be
 * populated with this text when the item is hovered.
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_TextFragment : public FFP_InventoryItemFragment
{
	GENERATED_BODY()

	FText GetText() const { return FragmentText; }
	void SetText(const FText& Text) { FragmentText = Text; }
	virtual void Assimilate(UFP_CompositeBase* Composite) const override;

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FText FragmentText;
};

/**
 * A labeled numeric stat for display in the item description (e.g. "Damage  14.5").
 * The value is rolled randomly between Min and Max the first time the item spawns in the
 * world (OnSpawned). Dropped and re-picked items retain the same rolled value.
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_LabeledNumberFragment : public FFP_InventoryItemFragment
{
	GENERATED_BODY()

	float GetValue() const { return Value; }

	virtual void Assimilate(UFP_CompositeBase* Composite) const override;

	/**
	 * Rolls Value between Min and Max on first world spawn.
	 * bRandomizeOnManifest is cleared afterward so re-pickups keep the same value.
	 */
	virtual void OnSpawned() override;

	// bRandomizeOnManifest starts true so the first spawn always rolls a value.
	// It is not a UPROPERTY so it is never reset by serialization — once false it stays false
	// for the lifetime of this manifest copy (inventory item or dropped actor).
	bool bRandomizeOnManifest{ true };

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FText Text_Label{};

	/** Current rolled value. VisibleAnywhere so designers can see it in PIE. */
	UPROPERTY(VisibleAnywhere, Category = "Inventory")
	float Value{ 0.f };

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float Min{ 0.f };

	UPROPERTY(EditAnywhere, Category = "Inventory")
	float Max{ 0.f };

	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bCollapseLabel{ false };

	UPROPERTY(EditAnywhere, Category = "Inventory")
	bool bCollapseValue{ false };

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MinFractionalDigits{ 1 };

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 MaxFractionalDigits{ 1 };
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
 * Generic consume modifier — a labeled numeric value applied when the item is consumed.
 * Inherit from this in Blueprint or C++ to implement specific consume effects (heal, buff, etc.).
 * Value is rolled at world spawn via OnSpawned() (inherited from FFP_LabeledNumberFragment).
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_ConsumeModifier : public FFP_LabeledNumberFragment
{
	GENERATED_BODY()

	/** Override to apply the modifier's effect when the item is consumed (e.g. restore HP). */
	virtual void OnConsume(APlayerController* PC) {}
};

/**
 * Consumable fragment. Holds an array of FFP_ConsumeModifier instances that define what
 * happens when the item is consumed (and what stats are shown in the description UI).
 * OnConsume, Assimilate, Manifest, and OnSpawned all delegate to the modifier array.
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_ConsumableFragment : public FFP_InventoryItemFragment
{
	GENERATED_BODY()

	void OnConsume(APlayerController* PC);

	virtual void Assimilate(UFP_CompositeBase* Composite) const override;
	virtual void Manifest() override;
	virtual void OnSpawned() override;

private:

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FFP_ConsumeModifier>> Modifiers;
};

// -------------------------------------------------------------------------
// Equipment
// -------------------------------------------------------------------------

/**
 * Base modifier for equipment stats. Inherits FFP_LabeledNumberFragment so its
 * value is rolled at spawn and displayed in the item description.
 * Override OnEquip/OnUnequip to apply/remove the stat effect.
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_EquipModifier : public FFP_LabeledNumberFragment
{
	GENERATED_BODY()

	virtual void OnEquip(APlayerController* PC) {}
	virtual void OnUnequip(APlayerController* PC) {}
};

/** Example equip modifier: prints a debug message scaled to the rolled Strength value. */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_StrengthModifier : public FFP_EquipModifier
{
	GENERATED_BODY()

	virtual void OnEquip(APlayerController* PC) override;
	virtual void OnUnequip(APlayerController* PC) override;
};

/**
 * Equipment fragment. Holds an array of FFP_EquipModifier instances applied when
 * the item is slotted into an equipped grid slot and removed when unequipped.
 * A bEquipped guard prevents double-application.
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_EquipmentFragment : public FFP_InventoryItemFragment
{
	GENERATED_BODY()

	void OnEquip(APlayerController* PC);
	void OnUnequip(APlayerController* PC);
	virtual void Assimilate(UFP_CompositeBase* Composite) const override;

private:

	bool bEquipped{ false };

	UPROPERTY(EditAnywhere, Category = "Inventory", meta = (ExcludeBaseStruct))
	TArray<TInstancedStruct<FFP_EquipModifier>> EquipModifiers;
};
