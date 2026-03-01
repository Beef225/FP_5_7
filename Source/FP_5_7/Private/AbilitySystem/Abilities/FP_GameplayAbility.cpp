#include "AbilitySystem/Abilities/FP_GameplayAbility.h"
#include "AbilitySystemComponent.h"

bool UFP_GameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, FGameplayTagContainer* OptionalRelevantTags) const
{
	if (bAllowNegativeCost)
	{
		return true;
	}
	return Super::CheckCost(Handle, ActorInfo, OptionalRelevantTags);
}

UAbilitySystemComponent* UFP_GameplayAbility::GetSourceASC() const
{
	return GetAbilitySystemComponentFromActorInfo();
}

float UFP_GameplayAbility::GetSourceAttributeValue(const FGameplayAttribute& Attribute, bool& bFound) const
{
	bFound = false;

	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC || !Attribute.IsValid())
	{
		return 0.f;
	}

	bFound = true;
	return ASC->GetNumericAttribute(Attribute);
}
