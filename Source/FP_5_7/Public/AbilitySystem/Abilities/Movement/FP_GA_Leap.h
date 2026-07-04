// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Templates/FP_GA_MovementSkill.h"
#include "FP_GA_Leap.generated.h"

class AFP_PlayerState;

/**
 * Leap — Movement skill.
 *
 * Teleports the character to the mouse cursor position, clamped to BaseLeapRange
 * (extended by the ExtraRange skill-tree passive).  If the cursor is within range
 * the character lands exactly there; if beyond range they land at the clamped point.
 *
 * C++ owns: leap target calculation (mouse trace + range clamp), passive reads,
 *           landing explosion.
 * Blueprint owns: CommitAbility, windup montage (conditioned on ShouldSkipWindup),
 *                 LaunchCharacter / RootMotion leap, ApplyLandingExplosion call,
 *                 VFX, EndAbility.
 *
 * Suggested BP flow:
 *   Event ActivateAbility
 *     → CommitAbility (fail → EndAbility)
 *     → GetLeapTarget  (cache for launch + explosion)
 *     → [if !ShouldSkipWindup] PlayMontageAndWait(Montage_Windup)
 *     → LaunchCharacter toward LeapTarget
 *     → WaitMovementModeChange (Walking) — lands
 *     → [if ShouldDoExplosiveLanding] ApplyLandingExplosion(CachedLeapTarget)
 *     → EndAbility
 */
UCLASS()
class FP_5_7_API UFP_GA_Leap : public UFP_GA_MovementSkill
{
	GENERATED_BODY()

public:
	UFP_GA_Leap();

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled) override;

	/**
	 * Traces under the cursor to find the ground position, then clamps to effective range.
	 * Convenience wrapper around ClampLeapTarget — use when not using TargetDataUnderMouse.
	 */
	UFUNCTION(BlueprintCallable, Category="Leap")
	FVector GetLeapTarget() const;

	/**
	 * Clamps a raw world hit location to effective leap range.
	 * Use this when the cursor trace comes from TargetDataUnderMouse:
	 *   ValidData → GetHitResultFromTargetData → ImpactPoint → ClampLeapTarget.
	 * If within range the location is returned unchanged.
	 * Beyond range: clamps XY and downward-traces for ground Z.
	 */
	UFUNCTION(BlueprintPure, Category="Leap")
	FVector ClampLeapTarget(FVector RawHitLocation) const;

	/** BaseLeapRange + SkillPassive.Leap.ExtraRange passive value (cm). */
	UFUNCTION(BlueprintPure, Category="Leap")
	float GetEffectiveRange() const;

	/** True when the NoWindup skill-tree node is allocated — skip Montage_Windup. */
	UFUNCTION(BlueprintPure, Category="Leap")
	bool ShouldSkipWindup() const;

	/** True when the ExplosiveLanding skill-tree node is allocated. */
	UFUNCTION(BlueprintPure, Category="Leap")
	bool ShouldDoExplosiveLanding() const;

	/**
	 * Sphere AoE damage centred on LandLocation.
	 * Radius = GetEffectiveSphereRadius(BaseExplosionRadius).
	 * Call from Blueprint on landing when ShouldDoExplosiveLanding() is true.
	 */
	UFUNCTION(BlueprintCallable, Category="Leap")
	void ApplyLandingExplosion(const FVector& LandLocation);

	// ---- Config ----------------------------------------------------------------

	/** Base leap range in cm. Extended by SkillPassive.Leap.ExtraRange. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Leap")
	float BaseLeapRange = 1200.f;

	/** Base landing explosion radius in cm. Scaled by AoE attribute modifiers. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Leap")
	float BaseExplosionRadius = 400.f;

	/** Windup animation played before the leap (skipped when NoWindup passive is active). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Leap|Montages")
	TObjectPtr<UAnimMontage> Montage_Windup;

	/** Blend-out speed once the ability ends and the camera releases back to normal follow. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Leap|Camera")
	float CameraReleaseInterpSpeed = 10.f;

private:
	const AFP_PlayerState* GetLocalPS() const;
};
