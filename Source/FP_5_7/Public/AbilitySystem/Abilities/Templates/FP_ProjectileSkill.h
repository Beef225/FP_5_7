// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/FP_GameplayAbility.h"
#include "FP_ProjectileSkill.generated.h"

class AFP_Projectile;

/**
 * Template GA for spawning projectiles.
 * Server-authoritative spawn and initialization (speed, etc.) based on the ability owner's ASC.
 */
UCLASS()
class FP_5_7_API UFP_ProjectileSkill : public UFP_GameplayAbility
{
	GENERATED_BODY()

protected:

	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	/** Spawns and initializes the projectile. Intended to be called from ActivateAbility (server only). */
	UFUNCTION(BlueprintCallable, Category="Projectile")
	void SpawnProjectile(const FVector& ProjectileTargetLocation);

	/** Which projectile to spawn. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Projectile")
	TSubclassOf<AFP_Projectile> ProjectileClass;
};
