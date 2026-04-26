// Copyright JG


#include "AbilitySystem/ModMagCalcs/FP_MMC_MaxHitPoints.h"

#include "AbilitySystem\FP_AttributeSet.h"
#include "Curves/BezierUtilities.h"
#include "Interaction/FP_CombatInterface.h"

UFP_MMC_MaxHitPoints::UFP_MMC_MaxHitPoints()
{
	FortitudeDef.AttributeToCapture = UFP_AttributeSet::GetFortitudeAttribute();
	FortitudeDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	FortitudeDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(FortitudeDef);

	IncreasedHitPointsDef.AttributeToCapture = UFP_AttributeSet::GetIncreasedHitPointsAttribute();
	IncreasedHitPointsDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	IncreasedHitPointsDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(IncreasedHitPointsDef);

	MoreHitPointsDef.AttributeToCapture = UFP_AttributeSet::GetMoreHitPointsAttribute();
	MoreHitPointsDef.AttributeSource = EGameplayEffectAttributeCaptureSource::Target;
	MoreHitPointsDef.bSnapshot = false;
	RelevantAttributesToCapture.Add(MoreHitPointsDef);
}

float UFP_MMC_MaxHitPoints::CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const
{
	// Gather tags from source and target
	const FGameplayTagContainer* SourceTags = Spec.CapturedSourceTags.GetAggregatedTags();
	const FGameplayTagContainer* TargetTags = Spec.CapturedTargetTags.GetAggregatedTags();
	
	FAggregatorEvaluateParameters EvaluationParameters;
	EvaluationParameters.SourceTags = SourceTags;
	EvaluationParameters.TargetTags = TargetTags;

	float Fortitude = 0.f;
	GetCapturedAttributeMagnitude(FortitudeDef, Spec, EvaluationParameters, Fortitude);
	Fortitude = FMath::Max<float>(Fortitude, 0.0f);

	float IncreasedHitPoints = 0.f;
	GetCapturedAttributeMagnitude(IncreasedHitPointsDef, Spec, EvaluationParameters, IncreasedHitPoints);

	float MoreHitPoints = 0.f;
	GetCapturedAttributeMagnitude(MoreHitPointsDef, Spec, EvaluationParameters, MoreHitPoints);

	IFP_CombatInterface* CombatInterface = Cast<IFP_CombatInterface>(Spec.GetContext().GetSourceObject());
	const int32 PlayerLevel = CombatInterface->GetPlayerLevel();

	const float Base = 65.f + 5.f * Fortitude + 8.f * PlayerLevel;
	return Base * (1.f + IncreasedHitPoints) * (1.f + MoreHitPoints);

}
