// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/FP_GameplayAbility.h"
#include "FP_GA_FreezePassive.generated.h"

/**
 * Passive ability that monitors the Heat attribute and applies chill/freeze effects.
 *
 * Chill zone  (Heat <= MinHeatThreshold * 0.75):  ramps MovementSpeed and SkillSpeed
 *                                                  from 0 down to -1 via an infinite GE.
 * Frozen state (Heat <= MinHeatThreshold):         applies State.Frozen tag, hard-zeroing
 *                                                  movement in CharacterBase::RefreshMovementSpeed.
 *
 * Requires a Blueprint GE assigned to FreezeModifiersGEClass with:
 *   - Duration: Infinite
 *   - Two Additive modifiers (MovementSpeed + SkillSpeed), both SetByCaller tag: SetByCaller.FreezeRamp
 */
UCLASS()
class FP_5_7_API UFP_GA_FreezePassive : public UFP_GameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		bool bReplicateEndAbility,
		bool bWasCancelled) override;

protected:
	// Blueprint GE to assign — Infinite duration, two Additive SetByCaller modifiers
	// (MovementSpeed + SkillSpeed), both using tag SetByCaller.FreezeRamp.
	UPROPERTY(EditDefaultsOnly, Category="Freeze")
	TSubclassOf<UGameplayEffect> FreezeModifiersGEClass;

private:
	void OnFreezeTimerTick();
	void UpdateFreezeState(float FreezeRamp);

	FActiveGameplayEffectHandle FreezeGEHandle;
	FTimerHandle FreezeTimerHandle;

	bool bIsChilled = false;
	bool bIsFrozen  = false;
};
