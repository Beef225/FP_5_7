// Copyright JG

#include "AbilitySystem/Abilities/Energy/FP_GA_Firebolt.h"
#include "FP_GameplayTags.h"

UFP_GA_Firebolt::UFP_GA_Firebolt()
{
	AbilityTags.AddTag(FFP_GameplayTags::Get().Skills_Energy_Firebolt);

	InstancingPolicy  = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}
