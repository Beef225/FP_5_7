// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "FP_GE_Blur.generated.h"

/**
 * Blur mastery (Flag.SkillTree.Blur). Infinite, applied by UFP_AbilitySystemComponent while the flag tag is present:
 *   ChanceToDodge  x2 (Multiply Compound — doubles the final value)
 *   Armour         Override 0   (no armour; shows as 0 in the UI)
 *   BlockChance    Override 0   (cannot block; shows as 0 in the UI)
 * Native class (not a transient object) so the active effect replicates.
 */
UCLASS()
class FP_5_7_API UFP_GE_Blur : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UFP_GE_Blur();
};
