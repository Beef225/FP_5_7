// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystem/Abilities/FP_GameplayAbility.h"
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

	// Grants a skill sourced from an equipped item. No input tag is assigned here —
	// the hotbar UI assigns one when the player slots the skill.
	FGameplayAbilitySpecHandle GrantItemSkill(TSubclassOf<UFP_GameplayAbility> AbilityClass, int32 Level = 1);

	// Removes a skill previously granted by GrantItemSkill.
	void RevokeItemSkill(FGameplayAbilitySpecHandle Handle);
	bool bStartupAbilitiesGiven = false;
	
	void AbilityInputTagHeld(const FGameplayTag& InputTag);
	void AbilityInputTagReleased(const FGameplayTag& InputTag);

	/** Atomically assigns InputTag to the skill identified by SkillTag.
	 *  Removes InputTag from any other spec that holds it, and strips any existing
	 *  InputTag-family tag from the target skill before adding the new one. */
	void AssignInputTagToSkill(const FGameplayTag& SkillTag, const FGameplayTag& InputTag);

	void UpgradeAttribute(const FGameplayTag& AttributeTag);

	UFUNCTION(Server, Reliable)
	void ServerUpgradeAttribute(const FGameplayTag& AttributeTag);
	
	void ForEachAbility(const FForEachAbilty& Delegate);
	
	static FGameplayTag GetAbilityTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);
	static FGameplayTag GetInputTagFromSpec(const FGameplayAbilitySpec& AbilitySpec);


protected:

	virtual void OnRep_ActivateAbilities() override;
	
	UFUNCTION(Client, Reliable)
	void ClientEffectApplied(UAbilitySystemComponent* AbilitySystemComponent, const FGameplayEffectSpec& EffectSpec, FActiveGameplayEffectHandle ActiveEffectHandle);
	
};
