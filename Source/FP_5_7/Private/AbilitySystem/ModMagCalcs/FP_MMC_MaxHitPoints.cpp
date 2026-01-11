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
	
	IFP_CombatInterface* CombatInterface = Cast<IFP_CombatInterface>(Spec.GetContext().GetSourceObject());
	const int32 PlayerLevel = CombatInterface->GetPlayerLevel();
	
	return 65.f + 5.f * Fortitude + 8.f * PlayerLevel; 

}
