// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Templates/FP_MeleeSkill.h"
#include "FP_GA_Impact.generated.h"

class AFP_PlayerState;

/**
 * Impact — Physical melee skill.
 *
 * C++ owns: CommitAbility, weapon-tip AoE overlap, damage, delayed chain scheduling.
 * Blueprint owns: montage, latent wait nodes, VFX, EndAbility.
 *
 * BP flow:
 *   Event ActivateAbility
 *     → CommitAbility (fail → EndAbility)
 *     → PlayMontageAndWait
 *         OnCompleted / OnInterrupted / OnCancelled → EndAbility
 *     → WaitGameplayEvent (Event.Montage.1)
 *         EventReceived → PerformInitialImpact
 *
 * Each enemy caught in the initial weapon-tip AoE detonates its own smaller AoE at its
 * location after GetEffectiveChainDelay() seconds. If the skill tree has raised the max
 * chain depth above the default (1), enemies caught by that secondary AoE detonate their
 * own AoE too, and so on. An enemy can only ever be hit once per activation (HitActors),
 * so chains can't loop back and forth indefinitely — they're bounded by both the exclude
 * list and the max chain depth.
 */
UCLASS()
class FP_5_7_API UFP_GA_Impact : public UFP_MeleeSkill
{
	GENERATED_BODY()

public:
	UFP_GA_Impact();

	/**
	 * Overlaps a sphere at the weapon-tip socket, damages everything caught in it, and
	 * schedules each hit enemy's chained detonation. Call this from BP after
	 * WaitGameplayEvent(Event.Montage.1) fires (i.e. at the swing's impact frame).
	 */
	UFUNCTION(BlueprintCallable, Category="Impact")
	void PerformInitialImpact();

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

	// ---- AoE ------------------------------------------------------------------

	/** Base radius (cm) of the initial AoE at the weapon tip. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Impact|AoE")
	float BaseInitialRadius = 350.f;

	/** Base radius (cm) of the secondary/chained AoE spawned at each hit enemy. Smaller than the initial AoE by default. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Impact|AoE")
	float BaseSecondaryRadius = 175.f;

	// ---- Chain ------------------------------------------------------------------

	/** Base seconds between a hit and its chained AoE detonation. Scaled by the SkillDuration attribute + skill-tree passive. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Impact|Chain")
	float BaseChainDelay = 0.75f;

	/** Floor on the effective chain delay so extreme reduced-duration sources can't collapse it to (or below) zero. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Impact|Chain")
	float MinChainDelay = 0.05f;

	/** Base number of chain generations beyond the initial hit. Default 1: initial AoE + one secondary generation per enemy caught, no further chaining. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Impact|Chain")
	int32 BaseMaxChainDepth = 1;

private:
	/** Overlaps a sphere at Origin, damages+schedules every newly-found enemy, excluding anything already in HitActors. */
	void ApplySphereDamageAndScheduleChain(const FVector& Origin, float Radius, float DamageMultiplier, int32 ChainDepth);

	/** Timer callback: detonates the chained AoE at TargetActor's current location, if it's still valid. */
	void SpawnChainedImpactAt(TWeakObjectPtr<AActor> TargetActor, int32 ChainDepth);

	float GetEffectiveInitialRadius() const;
	float GetEffectiveSecondaryRadius() const;
	float GetEffectiveChainDelay() const;
	int32 GetEffectiveMaxChainDepth() const;
	float GetEffectiveSecondaryDamageMultiplier() const;

	/** Convenience: returns the PlayerState, or null. */
	const AFP_PlayerState* GetLocalPS() const;

	// Persists across the whole activation, including all delayed chain generations —
	// an enemy that already spawned/received an AoE this activation cannot be retriggered.
	UPROPERTY()
	TArray<AActor*> HitActors;

	// Cleared in EndAbility so a cancelled/interrupted activation can't fire a "ghost" AoE later.
	TArray<FTimerHandle> PendingChainTimers;
};
