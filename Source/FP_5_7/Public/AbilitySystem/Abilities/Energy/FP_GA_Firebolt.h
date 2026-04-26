// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Abilities/Templates/FP_ProjectileSkill.h"
#include "FP_GA_Firebolt.generated.h"

/**
 * Firebolt — energy projectile ability.
 * Parented to UFP_ProjectileSkill; custom logic added here as the skill evolves.
 */
UCLASS()
class FP_5_7_API UFP_GA_Firebolt : public UFP_ProjectileSkill
{
	GENERATED_BODY()

public:

	UFP_GA_Firebolt();
};
