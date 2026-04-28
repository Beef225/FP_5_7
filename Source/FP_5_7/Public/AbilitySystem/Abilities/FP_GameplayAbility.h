#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "FP_GameplayAbility.generated.h"

class UAbilitySystemComponent;



UCLASS()
class FP_5_7_API UFP_GameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()

public:
	UPROPERTY(EditDefaultsOnly, Category="Input", meta = (Categories = "InputTag"))
	FGameplayTag StartupInputTag;

	// If true, bypasses the GAS cost check that blocks activation when the cost would take an attribute below 0.
	// Use this for abilities whose cost attribute (e.g. Heat) is designed to go negative.
	UPROPERTY(EditDefaultsOnly, Category="Cost")
	bool bAllowNegativeCost = true;

	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

	/** When true, prints skill name, level, and XP to screen on activation. */
	UPROPERTY(EditDefaultsOnly, Category="Debug")
	bool bDebugActivation = true;

protected:
	UFUNCTION(BlueprintPure, Category="FP|GAS")
	UAbilitySystemComponent* GetSourceASC() const;

	UFUNCTION(BlueprintPure, Category="FP|GAS")
	float GetSourceAttributeValue(const FGameplayAttribute& Attribute, bool& bFound) const;
};
