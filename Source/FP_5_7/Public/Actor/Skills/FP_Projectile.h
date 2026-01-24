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
	void OnSphereOverlap(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

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
};
