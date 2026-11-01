// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/FP_GameplayAbility.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"
#include "FP_GA_DeferredDamagePassive.generated.h"

/**
 * OBSOLETE (2026-10-31): deferred damage is now handled natively.
 *   ExecCalc_Damage splits the deferred share AFTER all mitigation into the
 *   IncomingDeferredDamage meta attribute -> UFP_AttributeSet -> UFP_AbilitySystemComponent
 *   ::AddDeferredDamage (stacking, re-timeable instances, 10 Hz drip).
 * GameplayEvent.DeferredDamage is no longer sent, so this ability does nothing. Remove
 * GA_DeferredDamage from startup passives and delete GA_/GE_DeferredDamage in the editor;
 * then this class can be deleted.
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
