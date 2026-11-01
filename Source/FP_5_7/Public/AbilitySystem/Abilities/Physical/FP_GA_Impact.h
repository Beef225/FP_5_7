// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Templates/FP_MeleeSkill.h"
#include "FP_GA_Impact.generated.h"

class AFP_PlayerState;
class UNiagaraSystem;

/**
 * Impact — Physical melee skill.
 *
 * C++ owns: CommitAbility, weapon-tip AoE overlap, damage, delayed chain scheduling, VFX/debug spawn.
 * Blueprint owns: montage, latent wait nodes, EndAbility.
 *
 * BP flow:
 *   Event ActivateAbility
 *     → CommitAbility (fail → EndAbility)
 *     → PlayMontageAndWait
 *         OnCompleted / OnInterrupted / OnCancelled → EndAbility
 *     → WaitGameplayEvent (Event.Montage.1)
 *         EventReceived → PerformInitialImpact
 *
 * EndAbility is expected to fire once the montage finishes — but the chain of delayed AoEs
 * is deliberately NOT tied to the ability's active lifetime. Chain state is threaded through
 * scheduled timers via a shared, per-activation context (FImpactChainStatePtr) rather than
 * stored on the ability instance, so: (a) explosions keep resolving after EndAbility, letting
 * the player act freely while they go off, and (b) if the player re-triggers Impact before a
 * previous activation's chain has finished, the two activations' exclude lists don't collide.
 *
 * Each enemy caught in the initial weapon-tip AoE detonates its own smaller AoE at its
 * location after GetEffectiveChainDelay() seconds. If the skill tree has raised the max
 * chain depth above the default (1), enemies caught by that secondary AoE detonate their
 * own AoE too, and so on. An enemy can only ever be hit once per activation, so chains
 * can't loop back and forth indefinitely — they're bounded by both the exclude list and
 * the max chain depth.
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

	// ---- VFX / Debug ------------------------------------------------------------

	/** Niagara system spawned (scaled to match the AoE radius) at every AoE trigger point — initial, secondary, and chained alike. Leave unset to skip VFX. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Impact|VFX")
	TObjectPtr<UNiagaraSystem> ImpactVFX;

	/** Radius (cm) ImpactVFX was authored at 1:1 scale. Actual spawn scale = triggered radius / this value. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Impact|VFX", meta=(ClampMin="1.0"))
	float VFXReferenceRadius = 100.f;

	/** Draws a debug sphere at every AoE trigger point (initial, secondary, chained). Toggle off once VFX is in place. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Impact|Debug")
	bool bDrawDebugAoESpheres = true;

private:
	/** Per-activation exclude-list context, threaded by value through scheduled timers so it outlives EndAbility and stays isolated per activation. */
	struct FImpactChainState
	{
		TArray<TWeakObjectPtr<AActor>> HitActors;
	};
	using FImpactChainStatePtr = TSharedPtr<FImpactChainState>;

	/** Overlaps a sphere at Origin, damages+schedules every newly-found enemy, excluding anything already in State's HitActors. */
	void ApplySphereDamageAndScheduleChain(const FImpactChainStatePtr& State, const FVector& Origin, float Radius, float DamageMultiplier, int32 ChainDepth);

	/** Timer callback: detonates the chained AoE at TargetActor's current location, if it's still valid. */
	void SpawnChainedImpactAt(FImpactChainStatePtr State, TWeakObjectPtr<AActor> TargetActor, int32 ChainDepth);

	/** Spawns ImpactVFX (if assigned) and draws a debug sphere (if enabled) at an AoE trigger point. */
	void SpawnImpactEffects(const FVector& Location, float Radius) const;

	float GetEffectiveInitialRadius() const;
	float GetEffectiveSecondaryRadius() const;
	float GetEffectiveChainDelay() const;
	int32 GetEffectiveMaxChainDepth() const;
	float GetEffectiveSecondaryDamageMultiplier() const;

	/** Convenience: returns the PlayerState, or null. */
	const AFP_PlayerState* GetLocalPS() const;
};
