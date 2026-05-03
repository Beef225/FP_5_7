// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "StructUtils/InstancedStruct.h"
#include "Libraries/FP_EnumDefs.h"
#include "SkillTree/FP_SkillTreeNodeEffect.h"
#include "FP_SkillTreeNodeData.generated.h"

class APlayerController;
class UTexture2D;

/**
 * Data asset describing a single node in a skill tree.
 *
 * NodeTag is the sole unique identifier and save key (e.g. PassiveTree.Might.IncreasedArmour.1).
 * ConnectedNodeTags lists all directly adjacent nodes — any allocated neighbour makes this node Available.
 * bIsStarterNode marks root nodes that are Available without requiring an allocated neighbour.
 *
 * Effects are applied when the node is allocated and reversed when deallocated.
 * Re-applying on session load is the tree widget's responsibility.
 */
UCLASS(BlueprintType)
class FP_5_7_API UFP_SkillTreeNodeData : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Applies all effects to the player. */
	void AllocateEffects(APlayerController* PC);

	/** Reverses all effects. */
	void DeallocateEffects(APlayerController* PC);

	// ---- Identity ----

	/** Unique tag for this node — used as identifier and save key.
	 *  Convention: PassiveTree.<Stat>.<EffectName>.<Index>  or  SkillTree.<Skill>.<EffectName>.<Index> */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Node")
	FGameplayTag NodeTag;

	// ---- Type & Layout ----

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Node")
	ESkillTreeNodeType NodeType = ESkillTreeNodeType::Minor;

	/** Canvas position written by the JSON importer. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Node|Layout")
	FVector2D NodePosition = FVector2D::ZeroVector;

	/** Tags of directly connected neighbouring nodes.
	 *  A node becomes Available when any neighbour is Allocated (or if bIsStarterNode). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Node|Layout")
	TArray<FGameplayTag> ConnectedNodeTags;

	/** If true, this node is Available at tree open with no allocated neighbour required.
	 *  Set on root/entry nodes of the tree. */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Node|Layout")
	bool bIsStarterNode = false;

	// ---- Display ----

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Node|Display")
	FText NodeName;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Node|Display", meta=(MultiLine=true))
	FText NodeDescription;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Node|Display")
	TObjectPtr<UTexture2D> NodeIcon;

	// ---- Effects ----

	UPROPERTY(EditDefaultsOnly, Category="Node|Effects", meta=(ExcludeBaseStruct))
	TArray<TInstancedStruct<FFP_SkillTreeNodeEffect>> Effects;
};
