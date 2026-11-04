// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/FP_GameplayAbility.h"
#include "FP_AbilitySystemComponent.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FAbilitesGiven, UFP_AbilitySystemComponent* );
DECLARE_DELEGATE_OneParam(FForEachAbilty, const FGameplayAbilitySpec& );

/** One deferred hit: damage still owed and the time left to deal it. Server-only. */
USTRUCT()
struct FFP_DeferredDamageInstance
{
	GENERATED_BODY()

	float RemainingDamage = 0.f;
	float RemainingTime = 0.f;
	/** Who dealt the original hit; drip ticks use it as instigator (kill / XP credit). */
	TWeakObjectPtr<UAbilitySystemComponent> SourceASC;
	/**
	 * Restorative Nanites (Flag.SkillTree.RecoupInsteadOfDefer): the hit was taken in full and
	 * RemainingDamage is HEALED back over RemainingTime instead of dealt. Still a deferred
	 * damage instance for everything that asks "am I deferring?" (e.g. Bide).
	 */
	bool bRecoup = false;
};


/**
 * 
 */
UCLASS()
class FP_5_7_API UFP_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:

	void AbilityActorInfoSet();
	
	FEffectAssetTags EffectAssetTags;
	FAbilitesGiven AbilitiesGivenDelegate;
	
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);

	// Grants a skill sourced from an equipped item. No input tag is assigned here —
	// the hotbar UI assigns one when the player slots the skill.
	FGameplayAbilitySpecHandle GrantItemSkill(TSubclassOf<UFP_GameplayAbility> AbilityClass, int32 Level = 1);

	// Removes a skill previously granted by GrantItemSkill.
	void RevokeItemSkill(FGameplayAbilitySpecHandle Handle);
	bool bStartupAbilitiesGiven = false;
	
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	/** Adds InputTag to the target skill's spec. Does not affect other specs. */
	void AddInputTagToSkill(const FGameplayTag& SkillTag, const FGameplayTag& InputTag);

	/** Removes a specific InputTag from the target skill's spec. Does not affect other specs. */
	void RemoveInputTagFromSkill(const FGameplayTag& SkillTag, const FGameplayTag& InputTag);

	void UpgradeAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);
	
	void ForEachAbility(const FForEachAbilty& Delegate);
	
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);

	// ---- Deferred damage -------------------------------------------------------------
	// Each direct hit taken while DeferredDamageAmount > 0 adds one instance (they stack).
	// Every tick each instance pays out RemainingDamage * dt / RemainingTime, so changing an
	// instance's RemainingTime re-spreads what is still owed. The owed AMOUNT never changes.
	// AUTOMATIC: whenever the DeferredDamageTime attribute changes (any GE, ability, node, gear),
	// every pending instance's remaining time is scaled by NewValue / OldValue. Dropping the
	// stat to 0 therefore pays out everything still owed on the next tick.

	/** Server only. Called from UFP_AttributeSet when ExecCalc_Damage outputs IncomingDeferredDamage. */
	void AddDeferredDamage(float Amount, float Duration, UAbilitySystemComponent* SourceASC);

	/** Set the remaining time of every pending instance (seconds). <= 0 pays everything out next tick. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "FP|Deferred Damage")
	void SetDeferredDamageRemainingTime(float NewRemainingTime);

	/** Multiply the remaining time of every pending instance (e.g. 2 = drip twice as slowly). */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "FP|Deferred Damage")
	void ScaleDeferredDamageRemainingTime(float Multiplier);

	/** Total damage still owed across all instances. */
	UFUNCTION(BlueprintPure, Category = "FP|Deferred Damage")
	float GetPendingDeferredDamage() const;

	UFUNCTION(BlueprintPure, Category = "FP|Deferred Damage")
	int32 GetNumDeferredDamageInstances() const { return DeferredDamageInstances.Num(); }

	/** Total life still to be restored by Restorative Nanites (recoup) instances. */
	UFUNCTION(BlueprintPure, Category = "FP|Deferred Damage")
	float GetPendingRecoup() const;

	/**
	 * Restorative Nanites: deferring does not mitigate — the hit lands in full and the deferred
	 * share is restored as life over DeferredDamageTime. Read by ExecCalc_Damage (no subtraction)
	 * and AddDeferredDamage (recoup instance).
	 */
	static bool RecoupsDeferredDamage(const UAbilitySystemComponent* ASC);

	/**
	 * Server only. Target-side "increased damage taken", summed into the ADDITIVE increased pool of
	 * ExecCalc_Damage and ExecCalc_DamageOverTime (alongside the attacker's increases). Not applied to
	 * deferred ticks. Bide (Flag.SkillTree.IncreasedDamageTaken.WhileDeferring): +BideDamageTakenWhileDeferring
	 * while at least one deferred damage instance is active.
	 */
	float GetIncreasedDamageTaken() const;

	static constexpr float BideDamageTakenWhileDeferring = 0.25f;

	// ---- Perpetual Motion (Flag.SkillTree.PerpetualMotion) ----------------------------------
	// Moving = avatar horizontal speed above the threshold (filters jitter / knockback settle).
	static constexpr float PerpetualMotionMovingSpeed   = 50.f;   // cm/s
	static constexpr float PerpetualMotionMovingMore    = 0.20f;  // x1.20 damage while moving
	static constexpr float PerpetualMotionStationaryLess = 0.30f; // x0.70 damage while stationary
	static constexpr float PerpetualMotionMovingDodge   = 0.10f;  // +10% dodge while moving (before the cap)

	static bool IsAvatarMoving(const UAbilitySystemComponent* ASC);
	/** Separate damage multiplier for the ATTACKER (1 without the flag). */
	static float GetPerpetualMotionDamageMultiplier(const UAbilitySystemComponent* SourceASC);
	/** Adjusts the TARGET's raw dodge chance: +MovingDodge while moving, 0 while stationary. */
	static float ApplyPerpetualMotionDodge(const UAbilitySystemComponent* TargetASC, float DodgeChance);

	// ---- Endothermic (Flag.SkillTree.Endothermic) ---------------------------------------------
	// Heat starts full and regenerates towards full (UFP_GE_EndothermicRegen); skills spend heat
	// (cooling skills restore it); skills are locked while Heat < 0. "Full" sits a hair under
	// MaxHeatThreshold so the Overheat check (Heat > Max) never trips at rest. Above Max the
	// normal Newton cooling (UFP_MMC_HeatDissapation) runs until Heat is back under Max.
	static constexpr float EndothermicRegenPerSecond = 0.20f;  // fraction of MaxHeatThreshold per second
	static constexpr float EndothermicFullEpsilon    = 0.01f;  // full = Max - this

	static bool IsEndothermic(const UAbilitySystemComponent* ASC);

	// ---- Iron Flesh (Flag.SkillTree.ArmourFromMaxHitPoints): UFP_GE_IronFlesh ----------------
	static constexpr float IronFleshArmourPerMaxHitPoint = 2.0f; // 200% of Maximum Hit Points as Armour

	// ---- Cascade Failure (Flag.SkillTree.SpreadStatusOnKill) ----------------------------------
	// Spreading lives in UFP_GA_DebuffPassive_Base::SpreadDebuffsOnDeath. Drawback: the owner deals
	// only Radiation damage (other types, Generic included, are zeroed in ExecCalc_Damage / DoT).
	static bool DealsOnlyRadiationDamage(const UAbilitySystemComponent* ASC);
	static float GetEndothermicFullHeat(float MaxHeatThreshold) { return FMath::Max(0.f, MaxHeatThreshold - EndothermicFullEpsilon); }
	/** True when Endothermic is active and Heat is below 0: skills cannot be used. */
	static bool IsEndothermicHeatLocked(const UAbilitySystemComponent* ASC);

	/** Drop all pending deferred damage without dealing it. */
	UFUNCTION(BlueprintCallable, BlueprintAuthorityOnly, Category = "FP|Deferred Damage")
	void ClearDeferredDamage();

	/**
	 * Shared instant GE used by UFP_GameplayAbility's native skill cost:
	 *   Heat      += SetByCaller.SkillCost.Heat
	 *   HitPoints += SetByCaller.SkillCost.HitPoints   (negative = HP paid)
	 * Built on first use (transient), so no per-skill cost GE assets are needed.
	 */
	UGameplayEffect* GetSkillCostEffect();

	/** Seconds between drip ticks. */
	UPROPERTY(EditDefaultsOnly, Category = "FP|Deferred Damage", meta = (ClampMin = "0.02"))
	float DeferredDamageTickInterval = 0.1f;


protected:

	virtual void OnRep_ActivateAbilities() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;
	
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);
	
private:
	void TickDeferredDamage();
	void OnDeferredDamageTimeChanged(const FOnAttributeChangeData& Data);
	FDelegateHandle DeferredDamageTimeChangedHandle;
	void ApplyDeferredDamageTick(float Amount, UAbilitySystemComponent* SourceASC);
	void ApplyRecoupTick(float Amount);

	// ---- Flag-driven effects -------------------------------------------------------------
	// Skill-tree Effect Flags are loose tags (FFP_NodeEffect_Flag). Flags whose effect is pure
	// attribute changes map to an infinite native GE here; it is applied while the tag count > 0.
	void RegisterFlagEffects();
	void OnFlagEffectTagChanged(const FGameplayTag Tag, int32 NewCount);
	TMap<FGameplayTag, TSubclassOf<UGameplayEffect>> FlagEffectClasses;
	TMap<FGameplayTag, FActiveGameplayEffectHandle> ActiveFlagEffects;

	TArray<FFP_DeferredDamageInstance> DeferredDamageInstances;
	FTimerHandle DeferredDamageTimer;
	double LastDeferredTickTime = 0.0;

	/** Transient instant GE: IncomingDamage += SetByCaller.DeferredDamagePerTick. Built on first use. */
	UPROPERTY(Transient)
	TObjectPtr<UGameplayEffect> DeferredDamageTickEffect;

	/** Transient instant GE: HitPoints += SetByCaller.DeferredDamagePerTick (Restorative Nanites). */
	UPROPERTY(Transient)
	TObjectPtr<UGameplayEffect> RecoupTickEffect;

	UPROPERTY(Transient)
	TObjectPtr<UGameplayEffect> SkillCostEffect;
};
