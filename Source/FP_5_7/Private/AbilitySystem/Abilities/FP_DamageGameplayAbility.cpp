// Copyright JG


#include "AbilitySystem/Abilities/FP_DamageGameplayAbility.h"

#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"

void UFP_DamageGameplayAbility::CauseDamage(AActor* TargetActor)
{FGameplayEffectSpecHandle DamageSpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass, 1.f);
	AssignRolledDamageMagnitudes(DamageSpecHandle);
	GetAbilitySystemComponentFromActorInfo()->ApplyGameplayEffectSpecToTarget(*DamageSpecHandle.Data.Get(), 
		UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActor));
	
}

void UFP_DamageGameplayAbility::AssignRolledDamageMagnitudes(FGameplayEffectSpecHandle& DamageSpecHandle) const
{
	for (const TTuple<FGameplayTag, FDamageRange>& Pair : DamageTypes)
	{
		const float MinScaledDamage = Pair.Value.DamageMin.GetValueAtLevel(GetAbilityLevel());
		const float MaxScaledDamage = Pair.Value.DamageMax.GetValueAtLevel(GetAbilityLevel());
		const float LowerBoundDamage = FMath::Min(MinScaledDamage, MaxScaledDamage);
		const float UpperBoundDamage = FMath::Max(MinScaledDamage, MaxScaledDamage);
		const float ScaledDamage = FMath::FRandRange(LowerBoundDamage, UpperBoundDamage);
		UAbilitySystemBlueprintLibrary::AssignTagSetByCallerMagnitude(DamageSpecHandle, Pair.Key, ScaledDamage);
	}
}
