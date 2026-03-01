// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/FP_GameplayAbility.h"
#include "FP_GA_OverheatPassive.generated.h"

/**
 * Passive ability that monitors the Heat attribute and applies the State.Overheat tag.
 * Intended to run permanently for the duration of the player character's lifetime.
 *
 * Overheat state (Heat > MaxHeatThreshold): applies State.Overheat tag.
 * The actual overheat damage GE is handled separately by the GA_Overheat Blueprint ability.
 */
UCLASS()
class FP_5_7_API UFP_GA_OverheatPassive : public UFP_GameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

private:
	void OnOverheatTimerTick();

	FTimerHandle OverheatTimerHandle;

	bool bIsOverheated = false;
};
