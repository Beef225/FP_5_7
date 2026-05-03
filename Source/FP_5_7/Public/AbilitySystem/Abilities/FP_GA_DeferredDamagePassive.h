// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/FP_GameplayAbility.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "FP_GA_DeferredDamagePassive.generated.h"

/**
 * Startup passive ability that intercepts the GameplayEvent.DeferredDamage event
 * broadcast by ExecCalc_Damage and applies the pre-computed per-tick damage as a
 * periodic gameplay effect.
 *
 * Setup:
 *   1. Grant this ability to the character at startup (same as FreezePassive/OverheatPassive).
 *   2. Set DeferredDamageEffectClass to the GE Blueprint configured as follows:
 *        - Duration Policy : Has Duration  (SetByCaller tag: SetByCaller.DotDuration)
 *        - Period          : 0.1
 *        - Modifier        : IncomingDamage += SetByCaller(SetByCaller.DeferredDamagePerTick)
 *        - No execution calculation needed.
 */
UCLASS()
class FP_5_7_API UFP_GA_DeferredDamagePassive : public UFP_GameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(
		const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

protected:

	/** Periodic GE that applies pre-scaled deferred damage at 10 Hz. */
	UPROPERTY(EditDefaultsOnly, Category="Deferred Damage")
	TSubclassOf<UGameplayEffect> DeferredDamageEffectClass;

private:
	UFUNCTION()
	void HandleDeferredDamageEvent(FGameplayEventData Payload);
};
