// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "GameplayAbilitySpec.h"
#include "FP_SkillTreeNodeEffect.generated.h"

class APlayerController;
class UGameplayEffect;

/**
 * Base instanced struct for all skill tree node effects.
 * OnAllocate is called when the player spends a point; OnDeallocate on refund / tree reset.
 * All display text lives on UFP_SkillTreeNodeData — effects are purely functional.
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_SkillTreeNodeEffect
{
	GENERATED_BODY()

	FFP_SkillTreeNodeEffect() {}
	virtual ~FFP_SkillTreeNodeEffect() {}

	virtual void OnAllocate(APlayerController* PC) {}
	virtual void OnDeallocate(APlayerController* PC) {}
};

// ---------------------------------------------------------------------------

/**
 * Applies a stat modifier at allocation time without needing a GE class asset.
 * The attribute is resolved at runtime from AttributeTag via UFP_AttributeSet::TagsToAttributes.
 *
 * Inkscape JSON block fields:  attribute_tag, modifier_op ("Add" | "Multiply"), magnitude.
 *
 * Additive:       magnitude applied as a flat bonus  (+5 armour)
 * Multiplicitive: magnitude applied as a multiplier  (0.05 = ×1.05, i.e. +5%)
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_NodeEffect_StatModifier : public FFP_SkillTreeNodeEffect
{
	GENERATED_BODY()

	virtual void OnAllocate(APlayerController* PC) override;
	virtual void OnDeallocate(APlayerController* PC) override;

	/** Tag matching an entry in UFP_AttributeSet::TagsToAttributes (e.g. Attributes.Secondary.Armour). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effect")
	FGameplayTag AttributeTag;

	/** How the magnitude combines with existing values. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effect")
	TEnumAsByte<EGameplayModOp::Type> ModifierOp = EGameplayModOp::Additive;

	/** Value to apply. Flat for Additive; fractional fraction for Multiplicitive (0.05 = +5%). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effect")
	float Magnitude = 0.f;

private:
	FActiveGameplayEffectHandle ActiveHandle;
};

// ---------------------------------------------------------------------------

/**
 * Grants a skill from the SkillLibrary when allocated; revokes it when deallocated.
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_NodeEffect_GrantSkill : public FFP_SkillTreeNodeEffect
{
	GENERATED_BODY()

	virtual void OnAllocate(APlayerController* PC) override;
	virtual void OnDeallocate(APlayerController* PC) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effect", meta=(Categories="Skills"))
	FGameplayTag SkillTag;

private:
	FGameplayAbilitySpecHandle GrantedHandle;
};

// ---------------------------------------------------------------------------

/**
 * Adds a loose gameplay tag to the player's ASC on allocation; removes it on deallocation.
 * Use this to gate unique code paths — query the ASC for the tag wherever needed.
 * The tag namespace should be e.g. Flag.SkillTree.X to keep it distinct.
 */
USTRUCT(BlueprintType)
struct FP_5_7_API FFP_NodeEffect_Flag : public FFP_SkillTreeNodeEffect
{
	GENERATED_BODY()

	virtual void OnAllocate(APlayerController* PC) override;
	virtual void OnDeallocate(APlayerController* PC) override;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Effect")
	FGameplayTag FlagTag;
};
