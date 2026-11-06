// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FP_EjectedCasing.generated.h"

class UStaticMeshComponent;
class USoundBase;

/**
 * Cosmetic physics debris thrown out of a weapon: shell casings, spent magazines, cells, ...
 * Spawned locally on each machine (not replicated) by UFP_WeaponFXLibrary::PlayWeaponFireFX,
 * launched once, then left to physics until LifeTime runs out.
 *
 * Make a Blueprint child per debris type and set its mesh. The mesh asset needs simple
 * collision (Static Mesh editor > Collision > Add Box/Capsule) or it falls through the floor.
 * Ignores pawns and projectiles so it never blocks characters or bullets.
 */
UCLASS()
class FP_5_7_API AFP_EjectedCasing : public AActor
{
	GENERATED_BODY()

public:
	AFP_EjectedCasing();

	/** Gives the casing its initial velocity (cm/s) and spin (deg/s, world axes). Call right after spawning. */
	UFUNCTION(BlueprintCallable, Category="Casing")
	void Launch(const FVector& LinearVelocity, const FVector& AngularVelocityDeg);

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Casing")
	TObjectPtr<UStaticMeshComponent> Mesh;

	/** Seconds before the casing is removed. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casing", meta=(ClampMin="0.1"))
	float LifeTime = 5.f;

	/** Mass override (kg). Light casings tumble and bounce nicely; 0 = use the mesh's computed mass. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casing", meta=(ClampMin="0.0"))
	float MassKg = 0.05f;

	/** Optional "tink" played on the first solid bounce. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casing")
	TObjectPtr<USoundBase> BounceSound;

	/** Impact speed (cm/s) needed to play BounceSound. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Casing", meta=(ClampMin="0.0"))
	float BounceSoundMinSpeed = 80.f;

protected:
	virtual void BeginPlay() override;

private:
	UFUNCTION()
	void OnMeshHit(UPrimitiveComponent* HitComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
	               FVector NormalImpulse, const FHitResult& Hit);

	bool bPlayedBounceSound = false;
};
