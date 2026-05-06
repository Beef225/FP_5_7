// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/FP_GA_DebuffPassive_Base.h"
#include "FP_GA_CorrodedPassive.generated.h"

UCLASS()
class FP_5_7_API UFP_GA_CorrodedPassive : public UFP_GA_DebuffPassive_Base
{
	GENERATED_BODY()

protected:
	virtual FGameplayTag      GetStateTag()           const override;
	virtual FGameplayTag      GetBuildupEventTag()    const override;
	virtual FGameplayAttribute GetThresholdAttribute() const override;
	virtual FGameplayAttribute GetDurationAttribute()  const override;

	virtual void OnDebuffApplied(UAbilitySystemComponent* TargetASC,
	                              UAbilitySystemComponent* SourceASC,
	                              float Duration) override;
	virtual void OnDebuffRemoved(UAbilitySystemComponent* TargetASC) override;

};
