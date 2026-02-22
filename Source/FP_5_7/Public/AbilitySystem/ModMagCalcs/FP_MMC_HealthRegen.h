// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameplayModMagnitudeCalculation.h"
#include "FP_MMC_HealthRegen.generated.h"

/**
 * 
 */
UCLASS()
class FP_5_7_API UFP_MMC_HealthRegen : public UGameplayModMagnitudeCalculation
{
	GENERATED_BODY()
	
public:
	UFP_MMC_HealthRegen();

	virtual float CalculateBaseMagnitude_Implementation(const FGameplayEffectSpec& Spec) const override;

private:

	FGameplayEffectAttributeCaptureDefinition MaxHitPointsDef;
	FGameplayEffectAttributeCaptureDefinition HealthRegenerationDef;

};
