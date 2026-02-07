// Copyright JG


#include "AbilitySystem/FP_AbilitySystemGlobals.h"

#include "FP_AbilityTypes.h"

FGameplayEffectContext* UFP_AbilitySystemGlobals::AllocGameplayEffectContext() const
{
	
	return new FFP_GameplayEffectContext();
}
