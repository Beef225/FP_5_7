// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/FP_DamageGameplayAbility.h"
#include "Actor/Skills/FP_SplineVFXActor.h"
#include "FP_GA_LightningCoil.generated.h"

class AFP_PlayerState;

/**
 * Lightning Coil — Chemical skill.
 *
 * C++ owns: CommitAbility, bolt math, AoE overlap, damage, chaining.
 * Blueprint owns: montage, latent wait nodes, debug drawing, VFX, EndAbility.
 *
 * BP flow:
 *   Event ActivateAbility
 *     → CommitAbility (fail → EndAbility)
 *     → DrawDebugCone   (immediate cone preview)
 *     → PlayMontageAndWait
 *         OnCompleted / OnInterrupted / OnCancelled → EndAbility
 *     → WaitGameplayEvent (Event.Montage.1)
 *         EventReceived → ForLoop 0..GetEffectiveNumBolts-1
 *             → GetRandomConePoint → DrawDebugSphere → FireBoltAtLocation
 */
UCLASS()
class FP_5_7_API UFP_GA_LightningCoil : public UFP_DamageGameplayAbility
{
	GENERATED_BODY()

public:
	UFP_GA_LightningCoil();

	/**
	 * Returns a random world-space point within the effective cone.
	 * Call once per bolt in the BP loop to get each impact location.
	 */
	UFUNCTION(BlueprintPure, Category="LightningCoil")
	FVector GetRandomConePoint(const FVector& Origin, const FVector& ForwardDir) const;

	/**
	 * Applies AoE damage + optional chain for a single bolt landing at ImpactCenter.
	 * Call this after drawing the debug sphere so gameplay and visuals are in sync.
	 */
	UFUNCTION(BlueprintCallable, Category="LightningCoil")
	void FireBoltAtLocation(const FVector& ImpactCenter);

protected:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	// ---- Cone ---------------------------------------------------------------

	/** Base distance (cm) the cone extends from the weapon tip. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LightningCoil|Cone")
	float ConeLength = 400.f;

	/** Base half-angle of the cone in degrees. Full spread = 2 × this value. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LightningCoil|Cone")
	float ConeHalfAngle = 25.f;

	// ---- Bolts --------------------------------------------------------------

	/** Number of bolts fired per activation (base). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LightningCoil|Bolts")
	int32 NumBolts = 3;

	/**
	 * AoE impact radius (cm) per bolt.
	 * Replace with GetEffectiveSphereRadius() once the SkillLibrary entry is created.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LightningCoil|Bolts")
	float BoltAoERadius = 50.f;

	// ---- Chain (wired, disabled by default) ---------------------------------

	/** Enable bolt chaining to nearby targets on hit. Activate via skill tree node. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LightningCoil|Chain")
	bool bCanChain = false;

	/** Maximum number of chain jumps per bolt. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LightningCoil|Chain")
	int32 MaxChainCount = 2;

	/** Radius (cm) searched for the next chain target after each hit. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LightningCoil|Chain")
	float ChainRadius = 500.f;

	// ---- VFX ----------------------------------------------------------------

	/**
	 * AFP_LightningBoltActor subclass to spawn per bolt.
	 * Create a Blueprint child of AFP_LightningBoltActor, assign your
	 * Niagara system asset in its defaults, then point this property at it.
	 */
	UPROPERTY(EditDefaultsOnly, Category="LightningCoil|VFX")
	TSubclassOf<AFP_SplineVFXActor> BoltActorClass;

	/**
	 * Maximum perpendicular offset (cm) applied to the spline midpoint.
	 * Adds a subtle curve to the bolt so it doesn't look perfectly straight.
	 * Set to 0 to disable.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LightningCoil|VFX")
	float BoltMidpointMaxOffset = 80.f;

	/** Number of intermediate spline points per bolt. 2 gives a natural jagged arc. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LightningCoil|VFX")
	int32 BoltMidPoints = 2;

	/**
	 * Spawns the bolt VFX actor from Start to End.
	 * Configures its spline before BeginPlay so the Niagara effect reads
	 * the correct path from the first frame. Optionally adds a randomised
	 * midpoint for a more natural arc.
	 */
	UFUNCTION(BlueprintCallable, Category="LightningCoil|VFX")
	void SpawnBoltVFX(const FVector& Start, const FVector& End);

	// ---- Effective parameter getters ----------------------------------------

	/**
	 * Effective bolt count after skill-tree modifiers.
	 * Cone size is handled automatically via GetEffectiveConeParams (base class).
	 * Override in BP to apply skill-tree attribute reads when those nodes are built.
	 */
	UFUNCTION(BlueprintNativeEvent, BlueprintPure, Category="LightningCoil|Bolts")
	int32 GetEffectiveNumBolts() const;

	/** Current effective cone half-angle in degrees, accounting for convergence and widening. Use this for debug draws instead of ConeHalfAngle. */
	UFUNCTION(BlueprintPure, Category="LightningCoil|Cone")
	float GetEffectiveConeHalfAngle() const { return GetCurrentConeAngle(); }

	// ---- Convergence --------------------------------------------------------

	/** Minimum cone half-angle (degrees) the convergence system can reach. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LightningCoil|Convergence", meta=(ClampMin="1.0"))
	float ConvergenceMinAngleDeg = 5.f;

	/**
	 * Seconds of inactivity before the cone begins widening back.
	 * Firing resets this timer instantly.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LightningCoil|Convergence", meta=(ClampMin="0.0"))
	float ConvergenceIdleDelaySec = 0.5f;

	/** Time (seconds) for the cone to widen fully back to ConeHalfAngle after the idle delay expires. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="LightningCoil|Convergence", meta=(ClampMin="0.1"))
	float ConvergenceWideningDurSec = 2.f;

protected:
	virtual void  CauseDamage(AActor* TargetActor) override;
	virtual int32 GetNumDamageRolls() const override;

private:
	void ApplyBoltAoEDamage(const FVector& ImpactCenter, TArray<AActor*>& InOutHitActors);
	void ChainBolt(const FVector& ImpactCenter, TArray<AActor*>& InOutHitActors, int32 RemainingChains);

	/** Convenience: returns the PlayerState, or null. */
	const AFP_PlayerState* GetLocalPS() const;

	/** ConeHalfAngle + SkillPassive.LightningCoil.ConeAngle passive. Convergence converges toward this. */
	float GetEffectiveBaseConeAngle() const;

	/** Reads SkillPassive.LightningCoil.ConvergenceRate from PlayerState; 0 if not allocated. */
	float GetConvergenceRate() const;

	/**
	 * Returns the current effective cone half-angle, accounting for convergence and
	 * the gradual widening that begins after ConvergenceIdleDelaySec of inactivity.
	 * Evaluated lazily at query time — no per-frame tick required.
	 */
	float GetCurrentConeAngle() const;

	/** Called by FireBoltAtLocation. Applies one convergence step and records the fire time. */
	void OnBoltFired();

	// Runtime convergence state — persists across activations (ability is InstancedPerActor).
	// -1 means "not yet initialised"; GetCurrentConeAngle returns ConeHalfAngle in that case.
	mutable float CurrentConeAngle = -1.f;
	float         LastBoltTime     = -1.f;
};
