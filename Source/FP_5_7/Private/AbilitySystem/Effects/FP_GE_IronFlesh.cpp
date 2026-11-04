// Copyright JG

#include "AbilitySystem/Effects/FP_GE_IronFlesh.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "AbilitySystem/FP_AbilitySystemComponent.h"

UFP_GE_IronFlesh::UFP_GE_IronFlesh()
{
	DurationPolicy = EGameplayEffectDurationType::Infinite;

	FAttributeBasedFloat FromMaxHitPoints;
	FromMaxHitPoints.Coefficient = FScalableFloat(UFP_AbilitySystemComponent::IronFleshArmourPerMaxHitPoint);
	FromMaxHitPoints.BackingAttribute = FGameplayEffectAttributeCaptureDefinition(
		UFP_AttributeSet::GetMaxHitPointsAttribute(), EGameplayEffectAttributeCaptureSource::Target, false);

	FGameplayModifierInfo Mod;
	Mod.Attribute = UFP_AttributeSet::GetArmourAttribute();
	Mod.ModifierOp = EGameplayModOp::AddBase;
	Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(FromMaxHitPoints);
	Modifiers.Add(Mod);
}
