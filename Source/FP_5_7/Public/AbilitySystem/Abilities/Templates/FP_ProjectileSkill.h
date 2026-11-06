// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/FP_DamageGameplayAbility.h"
#include "Actor/Skills/FP_Projectile.h"
#include "Libraries/FP_WeaponFXLibrary.h"
#include "FP_ProjectileSkill.generated.h"

class UGameplayEffect;

/**
 * Template GA for projectile skills. Server-authoritative spawn + hit resolution.
 *
 * Spawning:
 *   SpawnProjectile(TargetLocation)      original single-shot path (fires along the avatar's facing).
 *   SpawnProjectileInDirection(...)      explicit origin/direction; used by multi-shot skills.
 * Both set up speed (ProjectileSpeed attribute + GetExtraProjectileSpeedDelta), pierce and chain
 * (GetEffectivePierceCount / GetEffectiveChainCount) and point the projectile back at this skill.
 *
 * Hits: the projectile calls HandleProjectileHit, which rolls this skill's damage for that hit,
 * scaled by GetProjectileHitDamageMultiplier, applies it, then calls OnProjectileHitApplied.
 * Override those two to add per-hit rules (falloff, on-hit debuffs, fragments, ...).
 *
 * Generic projectile stats (these are the first readers of the attributes):
 *   Projectile.Pierce  (ProjectileTargetsPierced) -> extra pierces
 *   Projectile.Bounce  (ProjectileTargetsBounced) -> extra chains
 *   Projectile.Count   (ProjectileCount)          -> GetAdditionalProjectileCount(); each skill decides what it means
 * All read through GetSourceAttributeValue, so gated versions (Projectile.Pierce.Rifle, ...) apply.
 */
UCLASS()
class FP_5_7_API UFP_ProjectileSkill : public UFP_DamageGameplayAbility
{
	GENERATED_BODY()

public:
	/** Called by the projectile (server) when it hits a valid enemy. */
	virtual void HandleProjectileHit(AFP_Projectile* Projectile, AActor* Target, const FHitResult& Hit,
	                                 const FFP_ProjectileHitInfo& Info);

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	/** Spawns and initializes the projectile. Intended to be called from ActivateAbility (server only). */
	UFUNCTION(BlueprintCallable, Category="Projectile")
	void SpawnProjectile(const FVector& ProjectileTargetLocation);

	/**
	 * Server only. Spawns a projectile at Origin flying along Direction (flattened to horizontal).
	 * Returns null on clients or failure. ShotIndex / ShotGroupId / Generation are passed back in each hit's info.
		 */
	UFUNCTION(BlueprintCallable, Category="Projectile")
	AFP_Projectile* SpawnProjectileInDirection(const FVector& Origin, const FVector& Direction,
		int32 ShotIndex = 0, int32 ShotGroupId = 0, int32 Generation = 0);

	/** As SpawnProjectileInDirection; the projectile never hits IgnoreActors (e.g. fragments skip the enemy that spawned them). */
	AFP_Projectile* SpawnProjectileIgnoring(const FVector& Origin, const FVector& Direction,
		int32 ShotIndex, int32 ShotGroupId, int32 Generation, const TArray<AActor*>& IgnoreActors);

	/** Weapon socket location (Montage.Skill.Weapon) — where projectiles leave from. */
	UFUNCTION(BlueprintPure, Category="Projectile")
	FVector GetProjectileSpawnLocation() const;

	/** Pierces each projectile starts with: Projectile.Pierce stat + GetExtraPierceCount(). */
	UFUNCTION(BlueprintPure, Category="Projectile")
	int32 GetEffectivePierceCount() const;

	/** Chains each projectile starts with: Projectile.Bounce stat + GetExtraChainCount(). */
	UFUNCTION(BlueprintPure, Category="Projectile")
	int32 GetEffectiveChainCount() const;

	/** Projectile.Count stat, rounded. What an extra projectile means is up to each skill. */
	UFUNCTION(BlueprintPure, Category="Projectile")
	int32 GetAdditionalProjectileCount() const;

	/** Skill-specific additions (skill tree). */
	virtual int32 GetExtraPierceCount() const { return 0; }
	virtual int32 GetExtraChainCount() const { return 0; }
	virtual float GetExtraProjectileSpeedDelta() const { return 0.f; }
	virtual float GetEffectiveChainRadius() const { return BaseChainRadius; }

	/** Damage multiplier for one hit. Default 1. */
	virtual float GetProjectileHitDamageMultiplier(const AFP_Projectile* Projectile, const AActor* Target,
	                                               const FFP_ProjectileHitInfo& Info) const { return 1.f; }

	/** After the hit's damage was applied (server). Target may have died. */
	virtual void OnProjectileHitApplied(AFP_Projectile* Projectile, AActor* Target, const FHitResult& Hit,
	                                    const FFP_ProjectileHitInfo& Info) {}

	/**
	 * Plays FireFX (muzzle flash, fire sound, casing) on the avatar's weapon. Local cosmetic —
	 * call it on every machine that runs the shot (not server-only). Burst Fire calls it per round.
	 */
	UFUNCTION(BlueprintCallable, Category="Projectile|FX")
	void PlayFireFX() const;

	/** Per-shot cosmetics. See FFP_WeaponFireFX; leave fields empty to skip them. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile|FX")
	FFP_WeaponFireFX FireFX;

	/** Which projectile to spawn. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Projectile")
	TSubclassOf<AFP_Projectile> ProjectileClass;

	/** Radius (cm) a chaining projectile searches for its next target, before skill bonuses. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	float BaseChainRadius = 600.f;

private:
	/** Shared setup for both spawn paths; call between SpawnActorDeferred and FinishSpawning. */
	void InitProjectile(AFP_Projectile* Projectile, const FVector& TargetLocation);
};
