// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "FP_MMC_MaxHitPoints.generated.h"

/**
 * 
 */
UCLASS()
class FP_5_7_API UFP_MMC_MaxHitPoints : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	UFP_MMC_MaxHitPoints();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:

	FGameplayEffectAttributeCaptureDefinition FortitudeDef;

};
