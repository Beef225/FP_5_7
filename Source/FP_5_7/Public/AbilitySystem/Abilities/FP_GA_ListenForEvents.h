// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/FP_GameplayAbility.h"
#include "FP_GA_ListenForEvents.generated.h"

struct FGameplayEventData;

/**
 * Passive ability that listens for gameplay events and applies a gameplay effect
 * in response. Currently handles MetaAttribute_IncomingXP to feed the IncomingXP
 * meta attribute pipeline. Will be expanded to handle additional event-based rewards.
 *
 * Add to StartupPassiveAbilities. Never calls EndAbility — runs for character lifetime.
 * Set IncomingXPEffectClass on the Blueprint child class.
 */
UCLASS()
class FP_5_7_API UFP_GA_ListenForEvents : public UFP_GameplayAbility
{
	GENERATED_BODY()

public:
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo,
		const FGameplayEventData* TriggerEventData) override;

private:
	UFUNCTION()
	void OnXPEventReceived(FGameplayEventData Payload);

	/** GE that adds to IncomingXP via SetByCaller (tag: MetaAttribute.IncomingXP). Set on BP child. */
	UPROPERTY(EditDefaultsOnly, Category="Effects")
	TSubclassOf<UGameplayEffect> IncomingXPEffectClass;
};
