// Copyright JG

#include "UI/Widget/SkillTree/FP_SkillTreeNode.h"
#include "SkillTree/FP_SkillTreeNodeData.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Engine/Texture2D.h"

void UFP_SkillTreeNode::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_Node)
		Button_Node->OnClicked.AddDynamic(this, &UFP_SkillTreeNode::HandleButtonClicked);
}

void UFP_SkillTreeNode::Populate(const UFP_SkillTreeNodeData* InNodeData, ESkillTreeNodeState InState)
{
	NodeData = InNodeData;

	if (Image_NodeIcon && NodeData && NodeData->NodeIcon)
		Image_NodeIcon->SetBrushFromTexture(const_cast<UTexture2D*>(NodeData->NodeIcon.Get()));

	SetState(InState);
}

void UFP_SkillTreeNode::SetState(ESkillTreeNodeState InState)
{
	CurrentState = InState;
	OnStateChanged(CurrentState, NodeData ? NodeData->NodeType : ESkillTreeNodeType::Minor);
}

FGameplayTag UFP_SkillTreeNode::GetNodeTag() const
{
	return NodeData ? NodeData->NodeTag : FGameplayTag();
}

ESkillTreeNodeType UFP_SkillTreeNode::GetNodeType() const
{
	return NodeData ? NodeData->NodeType : ESkillTreeNodeType::Minor;
}

void UFP_SkillTreeNode::HandleButtonClicked()
{
	switch (CurrentState)
	{
	case ESkillTreeNodeState::Available:
		SetState(ESkillTreeNodeState::Pending);
		OnNodeClicked.Broadcast(this);
		break;

	case ESkillTreeNodeState::Pending:
		SetState(ESkillTreeNodeState::Available);
		OnNodeClicked.Broadcast(this);
		break;

	case ESkillTreeNodeState::Allocated:
	case ESkillTreeNodeState::Ineligible:
	default:
		break;
	}
}
