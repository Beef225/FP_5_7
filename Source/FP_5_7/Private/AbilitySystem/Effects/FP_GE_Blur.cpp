// Copyright JG

#include "AbilitySystem/Effects/FP_GE_Blur.h"
#include "AbilitySystem/FP_AttributeSet.h"

UFP_GE_Blur::UFP_GE_Blur()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	auto AddMod = [this](const FGameplayAttribute& Attribute, EGameplayModOp::Type Op, float Value)
	{
		FGameplayModifierInfo Mod;
		Mod.Attribute = Attribute;
		Mod.ModifierOp = Op;
		Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(FScalableFloat(Value));
		Modifiers.Add(Mod);
	};

	AddMod(UFP_AttributeSet::GetChanceToDodgeAttribute(), EGameplayModOp::MultiplyCompound, 2.f);
	AddMod(UFP_AttributeSet::GetArmourAttribute(),        EGameplayModOp::Override,         0.f);
	AddMod(UFP_AttributeSet::GetBlockChanceAttribute(),   EGameplayModOp::Override,         0.f);
}
