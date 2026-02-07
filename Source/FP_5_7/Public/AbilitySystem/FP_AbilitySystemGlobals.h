// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystemGlobals.h"
#include "FP_AbilitySystemGlobals.generated.h"

/**
 * 
 */
UCLASS()
class FP_5_7_API UFP_AbilitySystemGlobals : public UAbilitySystemGlobals
{
	GENERATED_BODY()
	
	virtual FGameplayEffectContext* AllocGameplayEffectContext() const override;
};
