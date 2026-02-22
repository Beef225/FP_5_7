// Copyright JG


#include "AbilitySystem/ModMagCalcs/FP_MMC_HealthRegen.h"
#include "AbilitySystem/FP_AttributeSet.h"

UFP_MMC_HealthRegen::UFP_MMC_HealthRegen()
{
	MaxHitPointsDef.AttributeToCapture = UFP_AttributeSet::GetMaxHitPointsAttribute();
	MaxHitPointsDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	MaxHitPointsDef.bSnapshot = false;

	HealthRegenerationDef.AttributeToCapture = UFP_AttributeSet::GetHealthRegenerationAttribute();
	HealthRegenerationDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	HealthRegenerationDef.bSnapshot = false;

	RelevantAttributesToCapture.Add(MaxHitPointsDef);
	RelevantAttributesToCapture.Add(HealthRegenerationDef);
}

float UFP_MMC_HealthRegen::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();

	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float MaxHitPoints = 0.f;
	GetCapturedAttributeMagnitude(MaxHitPointsDef, Spec, EvaluationParameters, MaxHitPoints);
	MaxHitPoints = FMath::Max(0.f, MaxHitPoints);

	float HealthRegeneration = 0.f;
	GetCapturedAttributeMagnitude(HealthRegenerationDef, Spec, EvaluationParameters, HealthRegeneration);
	HealthRegeneration = FMath::Max(0.f, HealthRegeneration);

	return MaxHitPoints * HealthRegeneration / 10.f;
}