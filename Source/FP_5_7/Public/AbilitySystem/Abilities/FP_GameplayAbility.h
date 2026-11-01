#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FP_GameplayAbility.generated.h"

class UAbilitySystemComponent;
class UFP_SkillLibrary;
struct FFP_AbilityEntry;



UCLASS()
class FP_5_7_API UFP_GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Input", meta = (Categories = "InputTag"))
	FGameplayTag StartupInputTag;

	// If true, bypasses the GAS cost check that blocks activation when the cost would take an attribute below 0.
	// Use this for abilities whose cost attribute (e.g. Heat) is designed to go negative.
	UPROPERTY(EditDefaultsOnly, Category="Cost")
	bool bAllowNegativeCost = true;

	/**
	 * NATIVE SKILL COST, driven by the Skill Library entry's BaseHeatCost and the library's
	 * HeatCostPerLevel. Formula and stats: see the SKILL HEAT COST block in FP_GameplayTags.h.
	 * Used whenever the skill has a library entry, unless BaseHeatCost is 0 AND this ability
	 * still has a Cost Gameplay Effect Class (legacy Blueprint cost).
	 * Liquid Cooled Systems: Might-weapon skills pay in HitPoints (heating or cooling).
	 * An HP cost >= current HP blocks the cast.
	 */
	UFUNCTION(BlueprintPure, Category="FP|Cost")
	void GetSkillCost(float& OutHeatCost, float& OutHitPointCost) const;

	/** This skill's Skill Library entry (found via its Skills.* asset tag and the PlayerState's library). */
	bool FindSkillEntry(const FGameplayAbilityActorInfo* ActorInfo, FFP_AbilityEntry& OutEntry,
	                    const UFP_SkillLibrary** OutLibrary = nullptr) const;

	/**
	 * Adds the derived attribute-weapon tags (SkillTagMod.MightWeapon, AgilityWeapon,
	 * FortitudeWeapon, ResonanceWeapon) for every weapon SkillTagMod in InOutTags, from the
	 * weapon/stat table: Mace, Ordnance -> Might; Sword, Pistol -> Agility;
	 * Gauntlets, Shotgun -> Fortitude; Conduit, DroneController -> Resonance;
	 * Rifle -> Might+Agility; Axe -> Might+Fortitude; Accelerator -> Might+Resonance;
	 * Spear -> Agility+Fortitude; Precision -> Agility+Resonance; Projector -> Fortitude+Resonance.
	 */
	static void AppendWeaponStatTags(FGameplayTagContainer& InOutTags);

	/**
	 * This skill's gated-stat tags (Skills.* + SkillTagMod.* incl. derived weapon-stat tags).
	 * Used by ExecCalc_DamageOverTime to evaluate gated stats for DoTs the ability applied.
	 */
	void GetSkillTagsForEffects(FGameplayTagContainer& OutTags) const { GetSkillModifierTags(OutTags); }

	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

protected:
	UFUNCTION(BlueprintPure, Category="FP|GAS")
	UAbilitySystemComponent* GetSourceASC() const;

	/**
	 * The caster's value of Attribute AS SEEN BY THIS SKILL: includes gated stat modifiers
	 * (e.g. Speed.Skill.Rifle) whose SkillTagMod requirements this skill's tags satisfy.
	 */
	UFUNCTION(BlueprintPure, Category="FP|GAS")
	float GetSourceAttributeValue(const FGameplayAttribute& Attribute, bool& bFound) const;

	/** Tags that qualify gated stat modifiers for this skill (SkillTagMod.*). Empty = ungated reads. */
	virtual void GetSkillModifierTags(FGameplayTagContainer& OutTags) const {}

	/** Flat heat added to HeatCost before multipliers (e.g. skill tree SkillPassive.<Skill>.HeatCost). */
	virtual float GetFlatHeatCostBonus(const FGameplayAbilityActorInfo* ActorInfo) const { return 0.f; }

private:
	/** False = no native cost for this skill (use the legacy Cost GE). OutHeatCost is signed. */
	bool ComputeSkillCost(const FGameplayAbilityActorInfo* ActorInfo, int32 Level,
	                      float& OutHeatCost, float& OutHitPointCost) const;
};
