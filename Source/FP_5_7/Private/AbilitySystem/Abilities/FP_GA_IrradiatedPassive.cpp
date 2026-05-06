// Copyright JG

#include "AbilitySystem/Abilities/FP_GA_IrradiatedPassive.h"

#include "AbilitySystemComponent.h"
#include "FP_GameplayTags.h"
#include "AbilitySystem/FP_AttributeSet.h"

FGameplayTag UFP_GA_IrradiatedPassive::GetStateTag() const
{
	return FFP_GameplayTags::Get().State_Irradiated;
}

FGameplayTag UFP_GA_IrradiatedPassive::GetBuildupEventTag() const
{
	return FFP_GameplayTags::Get().GameplayEvent_Debuff_Buildup_Irradiated;
}

FGameplayAttribute UFP_GA_IrradiatedPassive::GetThresholdAttribute() const
{
	return UFP_AttributeSet::GetIrradiatedThresholdAttribute();
}

FGameplayAttribute UFP_GA_IrradiatedPassive::GetDurationAttribute() const
{
	return UFP_AttributeSet::GetIrradiatedDurationAttribute();
}

void UFP_GA_IrradiatedPassive::OnDebuffApplied(UAbilitySystemComponent* TargetASC,
                                                UAbilitySystemComponent* SourceASC,
                                                float Duration)
{
	float Effectiveness = 1.f;
	if (SourceASC && SourceASC->GetSet<UFP_AttributeSet>())
	{
		const float Val = SourceASC->GetNumericAttribute(UFP_AttributeSet::GetIrradiatedEffectivenessAttribute());
		if (Val > 0.f) { Effectiveness = Val; }
	}

	const float Magnitude = -0.1f * Effectiveness;

	ApplyDebuffEffect(TargetASC, {
		{ UFP_AttributeSet::GetPhysicalDamageResistanceAttribute(),  Magnitude },
		{ UFP_AttributeSet::GetExplosiveDamageResistanceAttribute(), Magnitude },
		{ UFP_AttributeSet::GetRadiationDamageResistanceAttribute(), Magnitude },
		{ UFP_AttributeSet::GetChemicalDamageResistanceAttribute(),  Magnitude },
		{ UFP_AttributeSet::GetEnergyDamageResistanceAttribute(),    Magnitude },
		{ UFP_AttributeSet::GetOverheatDamageResistanceAttribute(),  Magnitude }
	});
}

void UFP_GA_IrradiatedPassive::OnDebuffRemoved(UAbilitySystemComponent* TargetASC)
{
	if (TargetASC && DebuffEffectHandle.IsValid())
	{
		TargetASC->RemoveActiveGameplayEffect(DebuffEffectHandle);
		DebuffEffectHandle.Invalidate();
	}
}
