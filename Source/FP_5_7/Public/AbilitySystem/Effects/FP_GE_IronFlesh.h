// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameplayEffect.h"
#include "FP_GE_IronFlesh.generated.h"

/**
 * Iron Flesh mastery (Flag.SkillTree.ArmourFromMaxHitPoints). Infinite, applied by UFP_AbilitySystemComponent
 * while the flag tag is present: Armour += IronFleshArmourPerMaxHitPoint x MaxHitPoints.
 * Attribute-based and not snapshotted, so it follows Maximum Hit Points live (the node's own 10% reduced
 * Hit Points is already included). Added to the flat Armour attribute, so Increased Armour scales it.
 */
UCLASS()
class FP_5_7_API UFP_GE_IronFlesh : public UGameplayEffect
{
	GENERATED_BODY()

public:
	UFP_GE_IronFlesh();
};
