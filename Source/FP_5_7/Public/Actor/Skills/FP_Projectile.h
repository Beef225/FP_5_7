// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "NiagaraFunctionLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameplayEffectTypes.h"
#include "FP_Projectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UNiagaraSystem;
class UFP_ProjectileSkill;

/** What a projectile knew about itself when it hit a target. Passed to UFP_ProjectileSkill::HandleProjectileHit. */
USTRUCT(BlueprintType)
struct FFP_ProjectileHitInfo
{
	GENERATED_BODY()

	/** 0 for the first enemy this projectile hit, 1 for the second, ... */
	UPROPERTY(BlueprintReadOnly) int32 HitIndex = 0;

	/** Targets this projectile had already pierced before this hit. */
	UPROPERTY(BlueprintReadOnly) int32 PiercesSoFar = 0;

	/** Times this projectile had already chained (redirected to a new enemy) before this hit. */
	UPROPERTY(BlueprintReadOnly) int32 ChainsSoFar = 0;

	/** Index of this projectile within its volley / burst (set by the skill that fired it). */
	UPROPERTY(BlueprintReadOnly) int32 ShotIndex = 0;

	/** Id of the volley / burst it belongs to (set by the skill). */
	UPROPERTY(BlueprintReadOnly) int32 ShotGroupId = 0;

	/** 0 = fired by the skill; 1+ = spawned by another projectile (fragments, splits). */
	UPROPERTY(BlueprintReadOnly) int32 Generation = 0;
};

/**
 * Base projectile for projectile skills.
 *
 * Hit flow (server): overlap / sweep-assist finds an actor ->
 *   - world geometry (no ASC): projectile stops.
 *   - dead, friendly, already hit by this projectile: ignored, keeps flying.
 *   - valid enemy: damage (through OwningSkill->HandleProjectileHit when set, else the
 *     pre-made DamageEffectSpecHandle), then
 *       pierce left  -> keep flying,
 *       else chain left -> turn toward the nearest un-hit enemy within ChainRadius,
 *       else stop.
 * Pierce, chain and the owning skill are set by UFP_ProjectileSkill before FinishSpawning.
 */
UCLASS()
class FP_5_7_API AFP_Projectile : public AActor
{
	GENERATED_BODY()

public:
	AFP_Projectile();

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	/** Fallback damage, applied when there is no OwningSkill to resolve the hit. */
	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

	/** Base (unmodified) initial speed for this projectile. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	float BaseInitialSpeed = 550.f;

	/** Base (unmodified) max speed for this projectile. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	float BaseMaxSpeed = 550.f;

	/** Destroyed after travelling this far (cm). 0 = only LifeSpan limits it. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	float MaxTravelDistance = 0.f;

	/** Initialize speed values (call before FinishSpawning ideally). */
	void InitSpeedFromDelta(float SpeedDelta);

	/** Set the actor that spawned/fired this projectile (call before FinishSpawning ideally). */
	void SetSourceActor(AActor* InSourceActor);

	// ---- Set by the firing skill before FinishSpawning --------------------------------------

	/** Enemies this projectile passes through before it can chain or stop. */
	UPROPERTY(BlueprintReadWrite, Category="Projectile|Hits")
	int32 PierceRemaining = 0;

	/** Times this projectile redirects to a new enemy after its pierces are used up. */
	UPROPERTY(BlueprintReadWrite, Category="Projectile|Hits")
	int32 ChainRemaining = 0;

	/** Search radius (cm) for the next chain target, around the enemy just hit. */
	UPROPERTY(BlueprintReadWrite, Category="Projectile|Hits")
	float ChainRadius = 600.f;

	UPROPERTY(BlueprintReadWrite, Category="Projectile|Hits")
	int32 ShotIndex = 0;

	UPROPERTY(BlueprintReadWrite, Category="Projectile|Hits")
	int32 ShotGroupId = 0;

	UPROPERTY(BlueprintReadWrite, Category="Projectile|Hits")
	int32 Generation = 0;

	/** Skill that resolves hits (damage, on-hit effects). Weak: a projectile can outlive its ability. */
	TWeakObjectPtr<UFP_ProjectileSkill> OwningSkill;

	/** This projectile never hits these (e.g. a fragment ignores the enemy its parent just killed). */
	void AddIgnoredTarget(AActor* Actor);

	/** Everything this projectile has hit so far (plus ignored targets). */
	TArray<AActor*> GetHitActors() const;

	/** True when Target is an enemy of whoever fired this (and alive). Used for hits and chain targets. */
	bool IsValidTarget(const AActor* Target) const;

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                     int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

private:
	/** Projectile has stopped (final hit or world hit); no further hits. */
	bool bHit = false;

	/** Set when a chain redirect happened this frame, so the sweep assist stops using stale hits. */
	bool bRedirectedThisFrame = false;

	int32 HitCount = 0;
	int32 PiercesUsed = 0;
	int32 ChainsUsed = 0;
	float DistanceTravelled = 0.f;

	UPROPERTY()
	TArray<TWeakObjectPtr<AActor>> HitActors;

	/** Actor that spawned/fired this projectile (used to ignore self-collisions). */
	UPROPERTY()
	TObjectPtr<AActor> SourceActor = nullptr;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<USphereComponent> Sphere;

	UPROPERTY(EditDefaultsOnly)
	float LifeSpan = 15.f;

	UPROPERTY(EditAnywhere)
	TObjectPtr<UNiagaraSystem> ImpactEffect;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> ImpactSound;

	UPROPERTY(EditAnywhere)
	TObjectPtr<USoundBase> LoopingSound;

	UPROPERTY()
	TObjectPtr<UAudioComponent> LoopingSoundComponent;

	// --- Top-down “hit assist” (server-side) ---

	/** Enables the sweep assist that detects targets below the projectile in top-down. */
	UPROPERTY(EditDefaultsOnly, Category="Projectile|HitAssist")
	bool bEnableSweepAssist = true;

	/** Extra radius scale for sweep shape vs the sphere component’s radius. */
	UPROPERTY(EditDefaultsOnly, Category="Projectile|HitAssist", meta=(ClampMin="0.5", ClampMax="3.0"))
	float SweepRadiusScale = 1.15f;

	/** How far down (in UU) to sweep to catch targets below the projectile. */
	UPROPERTY(EditDefaultsOnly, Category="Projectile|HitAssist", meta=(ClampMin="10.0", ClampMax="5000.0"))
	float DownSweepDistance = 300.f;

	/** Cached last location for forward sweep. */
	FVector PrevLocation = FVector::ZeroVector;

	/** Runs forward + downward sweeps to find valid targets even if projectile is above mesh. */
	void Server_TrySweepAssist();

	/** Shared impact handler used by overlap and sweeps. */
	void HandleImpact(AActor* HitActor, const FHitResult& Hit);

	bool HasAlreadyHit(const AActor* Actor) const;

	/** Turns toward the nearest valid, un-hit enemy within ChainRadius of From. False if none. */
	bool TryRedirectToChainTarget(const AActor* From);

	void PlayImpactCosmetics() const;

	/** Stops the projectile for good: sounds off, destroy on the server. */
	void StopProjectile();
};
