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
	bool bAllowNegativeCost = false;

	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;

protected:
	UFUNCTION(BlueprintPure, Category="FP|GAS")
	UAbilitySystemComponent* GetSourceASC() const;

	UFUNCTION(BlueprintPure, Category="FP|GAS")
	float GetSourceAttributeValue(const FGameplayAttribute& Attribute, bool& bFound) const;
};
