// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "FP_AbilitySystemComponent.generated.h"


DECLARE_MULTICAST_DELEGATE_OneParam(FEffectAssetTags, const FGameplayTagContainer& /*AssetTags*/);
DECLARE_MULTICAST_DELEGATE_OneParam(FAbilitesGiven, UFP_AbilitySystemComponent* );
DECLARE_DELEGATE_OneParam(FForEachAbilty, const FGameplayAbilitySpec& );


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
	FAbilitesGiven AbilitiesGivenDelegate;
	
	void AddCharacterAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupAbilities);
	void AddCharacterPassiveAbilities(const TArray<TSubclassOf<UGameplayAbility>>& StartupPassiveAbilities);
	bool bStartupAbilitiesGiven = false;
	
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);
	
	void ForEachAbility(const FForEachAbilty& Delegate);
	
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);


protected:

	virtual void OnRep_ActivateAbilities() override;
	
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);
	
};
