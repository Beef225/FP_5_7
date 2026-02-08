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
		
protected:
	UFUNCTION(BlueprintPure, Category="FP|GAS")
	UAbilitySystemComponent* GetSourceASC() const;

	UFUNCTION(BlueprintPure, Category="FP|GAS")
	float GetSourceAttributeValue(const FGameplayAttribute& Attribute, bool& bFound) const;
};
