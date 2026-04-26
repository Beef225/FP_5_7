// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/FP_DamageGameplayAbility.h"
#include "FP_GA_MovementSkill.generated.h"

/**
 * Base template for movement abilities.
 * Inherits damage support from UFP_DamageGameplayAbility so subclasses can
 * optionally deal damage during movement (e.g. a rolling dash that hits enemies).
 * For pure movement with no damage, simply leave DamageTypes empty.
 */
UCLASS(Abstract)
class FP_5_7_API UFP_GA_MovementSkill : public UFP_DamageGameplayAbility
{
	GENERATED_BODY()
};
