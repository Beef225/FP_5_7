// Copyright JG
//GitTest

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
	
public:
	UFUNCTION(BlueprintCallable)
	void CauseDamage(AActor* TargetActor);
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Damage|Skill")
	float GetSkillSpeedAttributeModifier() const;
	
	UFUNCTION(BlueprintCallable, BlueprintPure, Category = "Damage|Skill")
	float GetAoEAttributeModifier() const;


	
protected:
	void AppendSkillModifierTagsToDamageSpec(FGameplayEffectSpecHandle& DamageSpecHandle) const;
	
	void AssignRolledDamageMagnitudes(FGameplayEffectSpecHandle& DamageSpecHandle) const;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	UPROPERTY(EditDefaultsOnly, Category = "Damage", meta=(Categories="Damage"))
	TMap<FGameplayTag, FDamageRange> DamageTypes;
};
