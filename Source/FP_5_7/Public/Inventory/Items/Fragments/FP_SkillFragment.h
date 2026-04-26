// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Inventory/Items/Fragments/FP_ItemFragment.h"
#include "GameplayTagContainer.h"
#include "GameplayAbilitySpec.h"
#include "FP_SkillFragment.generated.h"

class APlayerController;
class UFP_SkillLibrary;

/**
 * One entry in the skill pool placed on an item by the designer.
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_SkillPoolEntry
{
	GENERATED_BODY()

	/** Tag matching an entry in the UFP_SkillLibrary (Categories: "Skills"). */
	UPROPERTY(EditAnywhere, Category="Skills", meta=(Categories="Skills"))
	FGameplayTag SkillTag;

	/** Level at which this skill is granted (passed to the ability spec). */
	UPROPERTY(EditAnywhere, Category="Skills", meta=(ClampMin=1))
	int32 SkillLevel{ 1 };
};

/**
 * Fragment that grants one or more skills to the player while this item is equipped.
 *
 * The designer places any number of skills in SkillPool and sets GrantCount to how many
 * of them should actually be granted. At spawn, GrantCount skills are drawn randomly
 * from the pool (without replacement) and stored in GrantedSkillTags. On equip those
 * skills are granted to the player's ASC; on unequip they are revoked.
 *
 * Input-tag assignment (hotbar slotting) is handled separately by the hotbar UI.
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_SkillFragment : public FFP_InventoryItemFragment
{
	GENERATED_BODY()

	const TArray<FGameplayTag>& GetGrantedSkillTags() const { return GrantedSkillTags; }

	virtual void OnSpawned(FFP_ItemManifest& Manifest) override;
	virtual void Assimilate(UFP_CompositeBase* Composite) const override;

	void OnEquip(APlayerController* PC);
	void OnUnequip(APlayerController* PC);

	/** All skills the designer has placed on this item. */
	UPROPERTY(EditAnywhere, Category="Skills")
	TArray<FFP_SkillPoolEntry> SkillPool;

	/** How many skills from the pool to randomly select at spawn. Clamped to pool size. */
	UPROPERTY(EditAnywhere, Category="Skills", meta=(ClampMin=1))
	int32 GrantCount{ 1 };

	/** Required for the item tooltip to display skill names and icons. */
	UPROPERTY(EditAnywhere, Category="Skills")
	TObjectPtr<UFP_SkillLibrary> SkillLibrary;

private:

	/** Skills rolled at spawn — tags and levels persisted with the item. */
	UPROPERTY(VisibleAnywhere, Category="Skills")
	TArray<FGameplayTag> GrantedSkillTags;

	UPROPERTY(VisibleAnywhere, Category="Skills")
	TArray<int32> GrantedSkillLevels;

	bool bRolled{ false };

	/** Handles for skills currently granted to the ASC — cleared on unequip. */
	TArray<FGameplayAbilitySpecHandle> ActiveHandles;
};
