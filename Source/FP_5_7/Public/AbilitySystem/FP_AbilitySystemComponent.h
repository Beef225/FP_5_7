// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "FP_AbilitySystemComponent.generated.h"

DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/);


/**
 * 
 */
UCLASS()
class FP_5_7_API UFP_AbilitySystemComponent : public UAbilitySystemComponent
{
	GENERATED_BODY()
	
public:

	void AbilityActorInfoSet();
	
	FEffectAssetTags EffectAssetTags;
	
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);
	
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);


protected:

	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);
	
};
