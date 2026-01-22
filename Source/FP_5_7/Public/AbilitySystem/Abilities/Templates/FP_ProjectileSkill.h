// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/FP_GameplayAbility.h"
#include "FP_ProjectileSkill.generated.h"

class AFP_Projectile;
/**
 * 
 */
UCLASS()
class FP_5_7_API UFP_ProjectileSkill : public UFP_GameplayAbility
{
	GENERATED_BODY()
	
protected:
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	TSubclassOf<AFP_Projectile> ProjectileClass;
};
