// Copyright JG

#include "AbilitySystem/ModMagCalcs/FP_MMC_EndothermicRegen.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "AbilitySystem/FP_AbilitySystemComponent.h"
#include "AbilitySystem/Effects/FP_GE_EndothermicRegen.h"

UFP_MMC_EndothermicRegen::UFP_MMC_EndothermicRegen()
{
	HeatDef.AttributeToCapture = UFP_AttributeSet::GetHeatAttribute();
	HeatDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	HeatDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(HeatDef);

	MaxHeatThresholdDef.AttributeToCapture = UFP_AttributeSet::GetMaxHeatThresholdAttribute();
	MaxHeatThresholdDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	MaxHeatThresholdDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(MaxHeatThresholdDef);
}

float UFP_MMC_EndothermicRegen::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	EvaluationParameters.TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	float Heat = 0.f;
	GetCapturedAttributeMagnitude(HeatDef, Spec, EvaluationParameters, Heat);
	float MaxHeat = 0.f;
	GetCapturedAttributeMagnitude(MaxHeatThresholdDef, Spec, EvaluationParameters, MaxHeat);
	MaxHeat = FMath::Max(0.f, MaxHeat);

	const float FullPoint = UFP_AbilitySystemComponent::GetEndothermicFullHeat(MaxHeat);
	if (Heat >= FullPoint)
	{
		return 0.f;
	}
	const float Step = UFP_AbilitySystemComponent::EndothermicRegenPerSecond * MaxHeat * UFP_GE_EndothermicRegen::RegenPeriod;
	return FMath::Min(Step, FullPoint - Heat);
}
