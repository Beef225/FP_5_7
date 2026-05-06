// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/FP_GameplayAbility.h"
#include "GameplayTagContainer.h"
#include "AttributeSet.h"
#include "ActiveGameplayEffectHandle.h"
#include "FP_GA_DebuffPassive_Base.generated.h"

class UAbilitySystemComponent;

/**
 * Abstract base for the four accumulation-based debuff passives (Bruised, Concussed,
 * Irradiated, Corroded).  One subclass instance lives on every character as a startup ability.
 *
 * Flow:
 *   Skills send GameplayEvent.Debuff.Buildup.X to the TARGET's ASC with EventMagnitude
 *   set to the raw buildup amount.  This passive intercepts those events and:
 *     1. Ignores them if the debuff is already active.
 *     2. Scales the amount by the post-debuff immunity ramp (linear 0→100% over 4 s).
 *     3. Adds the scaled amount to CurrentBuildup and resets the 3-second decay timer.
 *     4. If CurrentBuildup >= threshold (from target's own AS), applies the debuff.
 *
 *   Duration is read from the SOURCE character's ASC (Debuff.Duration.X attribute),
 *   defaulting to 4 s if the source has no valid AS.
 *
 *   Subclasses override the pure-virtual accessors to bind the right tags / attributes,
 *   and optionally override OnDebuffApplied / OnDebuffRemoved to add debuff-specific effects.
 */
UCLASS(Abstract)
class FP_5_7_API UFP_GA_DebuffPassive_Base : public UFP_GameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:

	// ---- Subclass interface -------------------------------------------------

	/** Tag added/removed on the target's ASC while the debuff is active. */
	virtual FGameplayTag GetStateTag() const
		PURE_VIRTUAL(UFP_GA_DebuffPassive_Base::GetStateTag, return FGameplayTag{};)

	/** GameplayEvent that carries incoming buildup for this debuff. */
	virtual FGameplayTag GetBuildupEventTag() const
		PURE_VIRTUAL(UFP_GA_DebuffPassive_Base::GetBuildupEventTag, return FGameplayTag{};)

	/** Attribute on the TARGET's AS that holds the threshold (base 100). */
	virtual FGameplayAttribute GetThresholdAttribute() const
		PURE_VIRTUAL(UFP_GA_DebuffPassive_Base::GetThresholdAttribute, return FGameplayAttribute{};)

	/** Attribute on the SOURCE's AS that holds the debuff duration (base 4 s). */
	virtual FGameplayAttribute GetDurationAttribute() const
		PURE_VIRTUAL(UFP_GA_DebuffPassive_Base::GetDurationAttribute, return FGameplayAttribute{};)

	/**
	 * Called when the debuff is first applied.
	 * TargetASC is this character's ASC; SourceASC is the instigating character's ASC
	 * (may be null if the source has no valid ASC).
	 */
	virtual void OnDebuffApplied(UAbilitySystemComponent* TargetASC,
	                              UAbilitySystemComponent* SourceASC,
	                              float Duration) {}

	/** Called when the debuff expires. */
	virtual void OnDebuffRemoved(UAbilitySystemComponent* TargetASC) {}

	/**
	 * Builds and applies an Infinite GE with one additive modifier per entry in Modifiers,
	 * stores the handle in DebuffEffectHandle.
	 * Modifiers: array of { Attribute, Value } pairs.
	 */
	void ApplyDebuffEffect(UAbilitySystemComponent* TargetASC,
	                       const TArray<TPair<FGameplayAttribute, float>>& Modifiers);

	/** Active GE handle set by ApplyDebuffEffect — used by subclasses to remove on expiry. */
	FActiveGameplayEffectHandle DebuffEffectHandle;

	/** Fallback duration (seconds) when the source has no valid attribute. */
	UPROPERTY(EditDefaultsOnly, Category="Debuff")
	float DefaultDuration = 4.f;

	/** Seconds of no new buildup before the accumulated buildup resets to zero. */
	UPROPERTY(EditDefaultsOnly, Category="Debuff")
	float BuildupDecayDelay = 3.f;

	/** Seconds over which the post-debuff immunity ramp goes from 0% to 100%. */
	UPROPERTY(EditDefaultsOnly, Category="Debuff")
	float ImmunityRampDuration = 4.f;

private:

	UFUNCTION()
	void HandleBuildupEvent(FGameplayEventData Payload);

	void ApplyDebuff(UAbilitySystemComponent* SourceASC);
	void RemoveDebuff();
	void OnDecayTimerExpired();
	void OnDebuffDurationExpired();

	float  CurrentBuildup              = 0.f;
	bool   bDebuffActive               = false;

	/** World time at which post-debuff immunity started; <0 means no immunity. */
	float  PostDebuffImmunityStartTime = -1.f;

	/** Cached source ASC used for OnDebuffApplied/Removed without re-resolving. */
	UPROPERTY()
	TObjectPtr<UAbilitySystemComponent> CachedSourceASC;

	FTimerHandle DecayTimerHandle;
	FTimerHandle DebuffDurationHandle;
};
