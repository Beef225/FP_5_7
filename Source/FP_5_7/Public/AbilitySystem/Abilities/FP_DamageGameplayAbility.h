// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "FP_GameplayAbility.h"
#include "FP_DamageGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class FP_5_7_API UFP_DamageGameplayAbility : public UFP_GameplayAbility
{
	GENERATED_BODY()
	
protected: 
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage")
	TMap<FGameplayTag, FScalableFloat> DamageTypes;
};
