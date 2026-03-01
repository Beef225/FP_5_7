// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/FP_GameplayAbility.h"
#include "FP_GA_FreezePassive.generated.h"

/**
 * Passive ability that monitors the Heat attribute and applies chill/freeze effects.
 * Intended to run permanently for the duration of the player character's lifetime.
 *
 * Chill zone  (Heat <= MinHeatThreshold * 0.75):  ramps MovementSpeed and SkillSpeed
 *                                                  down to 0 via FreezeRamp on CharacterBase.
 * Frozen state (Heat <= MinHeatThreshold):         applies State.Frozen tag, hard-zeroing
 *                                                  movement in CharacterBase::RefreshMovementSpeed.
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

private:
	void OnFreezeTimerTick();
	void UpdateFreezeState(float FreezeRamp);

	FTimerHandle FreezeTimerHandle;

	bool bIsChilled = false;
	bool bIsFrozen  = false;
};
