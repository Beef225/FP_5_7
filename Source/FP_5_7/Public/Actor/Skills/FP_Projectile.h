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

UCLASS()
class FP_5_7_API AFP_Projectile : public AActor
{
	GENERATED_BODY()

public:
	AFP_Projectile();

	virtual void Tick(float DeltaSeconds) override;

	UPROPERTY(VisibleAnywhere)
	TObjectPtr<UProjectileMovementComponent> ProjectileMovement;

	UPROPERTY(BlueprintReadWrite, meta = (ExposeOnSpawn = true))
	FGameplayEffectSpecHandle DamageEffectSpecHandle;

	/** Base (unmodified) initial speed for this projectile. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	float BaseInitialSpeed = 550.f;

	/** Base (unmodified) max speed for this projectile. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Projectile")
	float BaseMaxSpeed = 550.f;

	/** Initialize speed values (call before FinishSpawning ideally). */
	void InitSpeedFromDelta(float SpeedDelta);

	/** Set the actor that spawned/fired this projectile (call before FinishSpawning ideally). */
	void SetSourceActor(AActor* InSourceActor);

	UFUNCTION()
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	                     int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

protected:
	virtual void BeginPlay() override;
	virtual void Destroyed() override;

private:
	bool bHit = false;

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
};
