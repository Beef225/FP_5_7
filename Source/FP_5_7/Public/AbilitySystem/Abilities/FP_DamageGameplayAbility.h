// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "FP_GameplayAbility.h"
#include "FP_DamageGameplayAbility.generated.h"

USTRUCT(BlueprintType)
struct FDamageRange
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	FScalableFloat DamageMin = FScalableFloat();
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Damage")
	FScalableFloat DamageMax = FScalableFloat();
};



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
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage", meta=(Categories="Damage"))
	TMap<FGameplayTag, FScalableFloat> DamageTypes;
	
	//UPROPERTY(EditDefaultsOnly, Category = "Damage")
	//TMap<FGameplayTag, FDamageRange> DamageTypes;
};
