// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Templates/FP_ProjectileSkill.h"
#include "FP_GA_BurstFire.generated.h"

class AFP_PlayerState;

/**
 * Burst Fire — Physical rifle projectile skill. Fires a burst of rounds (base 3) along the
 * character's facing, one every BurstInterval seconds, each with a little random spread.
 *
 * C++ owns: CommitAbility, round timing, aim/spread, spawning, per-hit damage rules, ending the
 *           ability after the last round (bEndAbilityWhenBurstFinishes).
 * Blueprint owns (optional): montage / sounds. Minimal BP flow:
 *   Event ActivateAbility → PlayMontageAndWait (no End Ability on its pins needed)
 *   Do NOT call CommitAbility in the BP — C++ commits once per burst.
 *   With bAutoStartBurst off, call StartBurst yourself (e.g. on WaitGameplayEvent Event.Montage.1).
 *
 * Rounds per burst = BaseRoundsPerBurst + SkillPassive ExtraRounds + Projectile.Count stat.
 * Pierce / chain come from UFP_ProjectileSkill (Projectile.Pierce / Projectile.Bounce stats + tree).
 *
 * Skill tree (SkillPassive.BurstFire.*), all read here:
 *   ExtraRounds, BurstInterval (delta), Spread (delta), ProjectileSpeed (delta)
 *   PierceCount, PierceDamage (+retention per pierce), Overpenetration (more dmg per pierce instead)
 *   ChainCount, ChainRadius (metres), ChainDamage (+retention per chain)
 *   FinalRoundDamage (more), FocusFire (more per earlier hit on that enemy this burst)
 *   FragmentCount, FragmentDamage (on kill the round splits)
 *   Volley (>0: all rounds at once in a fan), BruisedBuildup (per hit)
 * Damage leaves (IncreasedDamage, MoreDamage, ...) go through AssignSkillPassiveBonuses.
 */
UCLASS()
class FP_5_7_API UFP_GA_BurstFire : public UFP_ProjectileSkill
{
	GENERATED_BODY()

public:
	UFP_GA_BurstFire();

	/** Starts firing the burst (server). Called automatically on activation when bAutoStartBurst. */
	UFUNCTION(BlueprintCallable, Category="BurstFire")
	void StartBurst();

	/** Rounds this burst will fire (base + tree + Projectile.Count). */
	UFUNCTION(BlueprintPure, Category="BurstFire")
	int32 GetEffectiveRoundsPerBurst() const;

	/** Seconds between rounds, after tree and skill speed. */
	UFUNCTION(BlueprintPure, Category="BurstFire")
	float GetEffectiveBurstInterval() const;

	/** Total burst duration (first to last round) — handy for montage timing. */
	UFUNCTION(BlueprintPure, Category="BurstFire")
	float GetBurstDuration() const;

protected:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility, bool bWasCancelled) override;

	// UFP_ProjectileSkill
	virtual int32 GetExtraPierceCount() const override;
	virtual int32 GetExtraChainCount() const override;
	virtual float GetExtraProjectileSpeedDelta() const override;
	virtual float GetEffectiveChainRadius() const override;
	virtual float GetProjectileHitDamageMultiplier(const AFP_Projectile* Projectile, const AActor* Target,
	                                               const FFP_ProjectileHitInfo& Info) const override;
	virtual void OnProjectileHitApplied(AFP_Projectile* Projectile, AActor* Target, const FHitResult& Hit,
	                                    const FFP_ProjectileHitInfo& Info) override;

	// ---- Burst ----------------------------------------------------------------

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BurstFire|Burst", meta=(ClampMin="1"))
	int32 BaseRoundsPerBurst = 3;

	/** Seconds between rounds before modifiers. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BurstFire|Burst", meta=(ClampMin="0.0"))
	float BaseBurstInterval = 0.09f;

	/** Floor for the interval, so stacked reductions can't collapse the burst into one frame. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BurstFire|Burst", meta=(ClampMin="0.0"))
	float MinBurstInterval = 0.025f;

	/** Interval also scales with skill speed (GetEffectiveSkillRate). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BurstFire|Burst")
	bool bIntervalScalesWithSkillSpeed = true;

	/** Fire as soon as the ability activates. Off = BP calls StartBurst (e.g. on a montage event). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BurstFire|Burst")
	bool bAutoStartBurst = true;

	/** End the ability right after the last round. Off = BP ends it (e.g. on montage completed). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BurstFire|Burst")
	bool bEndAbilityWhenBurstFinishes = true;

	// ---- Aim ------------------------------------------------------------------

	/** Random yaw jitter per round (degrees, each side) before modifiers. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BurstFire|Aim", meta=(ClampMin="0.0"))
	float BaseSpreadHalfAngle = 3.f;

	/** Scatter Volley: half-width (degrees) of the fan the rounds are spread across. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BurstFire|Aim", meta=(ClampMin="0.0"))
	float VolleyFanHalfAngle = 15.f;

	// ---- Hit rules ------------------------------------------------------------

	/** Damage kept per target pierced (0.8 = each later target takes 20% less). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BurstFire|Hits", meta=(ClampMin="0.0"))
	float BasePierceDamageRetention = 0.8f;

	/** Damage kept per ricochet. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BurstFire|Hits", meta=(ClampMin="0.0"))
	float BaseChainDamageRetention = 0.8f;

	/** Fragment damage as a fraction of a round's, before the tree. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BurstFire|Hits", meta=(ClampMin="0.0"))
	float BaseFragmentDamage = 0.5f;

	/** Half-width (degrees) of the fan fragments fly out in, centred on the round's direction. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="BurstFire|Hits", meta=(ClampMin="0.0"))
	float FragmentFanHalfAngle = 35.f;

private:
	void FireNextRound();
	void FireRound(int32 RoundIndex, const FVector& Direction);
	float GetEffectiveSpreadHalfAngle() const;
	bool IsVolley() const;

	float GetPassive(const FGameplayTag& Tag) const;
	const AFP_PlayerState* GetLocalPS() const;

	FTimerHandle BurstTimerHandle;
	int32 BurstId = 0;
	int32 RoundsFired = 0;
	int32 RoundsThisBurst = 0;

	/** Focus Fire: hits per enemy in the current burst. */
	TMap<TWeakObjectPtr<AActor>, int32> BurstHitCounts;
};
