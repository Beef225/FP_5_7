#include "AbilitySystem/Abilities/FP_GameplayAbility.h"
#include "AbilitySystemComponent.h"

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
