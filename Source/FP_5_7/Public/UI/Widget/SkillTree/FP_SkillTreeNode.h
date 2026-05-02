// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/FP_UserWidget.h"
#include "Libraries/FP_EnumDefs.h"
#include "GameplayTagContainer.h"
#include "FP_SkillTreeNode.generated.h"

class UButton;
class UImage;
class UFP_SkillTreeNodeData;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSkillTreeNodeClicked, class UFP_SkillTreeNode* /*Node*/);

/**
 * Base widget for a single skill tree node.
 *
 * Responsibilities:
 *   - Display the node's current state visually (via OnStateChanged BlueprintImplementableEvent).
 *   - Handle click input: Available → Pending, Pending → Available. Ineligible and Allocated
 *     clicks are ignored. The owning tree widget handles Allocated transitions.
 *   - Broadcast OnNodeClicked so the tree widget can react (e.g. show tooltip, manage pending set).
 *
 * Designers subclass this in Blueprint and implement OnStateChanged to drive visuals
 * (color tint, border, icon overlay, etc.) per node type and state.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class FP_5_7_API UFP_SkillTreeNode : public UFP_UserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Initialises the node with its data and computes its initial state.
	 * Called by the tree widget after spawning this widget.
	 */
	void Populate(const UFP_SkillTreeNodeData* InNodeData, ESkillTreeNodeState InState);

	/** Updates the displayed state without changing NodeData. */
	void SetState(ESkillTreeNodeState InState);

	FGameplayTag        GetNodeTag()      const;
	ESkillTreeNodeType  GetNodeType()     const;
	ESkillTreeNodeState GetCurrentState() const { return CurrentState; }

	const UFP_SkillTreeNodeData* GetNodeData() const { return NodeData; }

	/** Fired when the player clicks a non-ineligible node. The tree widget subscribes to this. */
	FOnSkillTreeNodeClicked OnNodeClicked;

protected:
	virtual void NativeConstruct() override;

	/**
	 * Called whenever the state changes. Implement in Blueprint to update visuals:
	 * tint, border style, icon overlay, etc.
	 * NodeType is passed so a single BP subclass can handle all three node sizes.
	 */
	UFUNCTION(BlueprintImplementableEvent, Category="Skill Tree Node")
	void OnStateChanged(ESkillTreeNodeState NewState, ESkillTreeNodeType Type);

	/** Exposes node data to Blueprint for tooltip and display logic. */
	UPROPERTY(BlueprintReadOnly, Category="Skill Tree Node")
	TObjectPtr<const UFP_SkillTreeNodeData> NodeData;

	/** Current display state. Read-only in Blueprint — use SetState() from C++. */
	UPROPERTY(BlueprintReadOnly, Category="Skill Tree Node")
	ESkillTreeNodeState CurrentState = ESkillTreeNodeState::Ineligible;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Node;

	/** Optional — set in Blueprint to show the node icon. */
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UImage> Image_NodeIcon;

private:
	UFUNCTION()
	void HandleButtonClicked();
};
