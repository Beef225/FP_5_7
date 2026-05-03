// Copyright JG

#include "UI/Widget/SkillTree/FP_SkillTreeNode.h"
#include "SkillTree/FP_SkillTreeNodeData.h"
#include "Components/Button.h"
#include "Components/Image.h"
#include "Components/SizeBox.h"
#include "Engine/Texture2D.h"

FVector2D UFP_SkillTreeNode::GetDesiredHalfSize() const
{
	if (SizeBox_Root)
	{
		const float W = SizeBox_Root->GetWidthOverride();
		const float H = SizeBox_Root->GetHeightOverride();
		// WidthOverride/HeightOverride return 0 when not set — fall through to default
		if (W > 0.f && H > 0.f)
			return FVector2D(W, H) * 0.5f;
	}
	return FVector2D(25.f);
}

void UFP_SkillTreeNode::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_Node)
		Button_Node->OnClicked.AddDynamic(this, &UFP_SkillTreeNode::HandleButtonClicked);
}

void UFP_SkillTreeNode::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!bPulseActive || !Image_NodeIcon) return;

	PulseTime += InDeltaTime;
	// Sine wave: oscillates 0→1→0 at PulseSpeed Hz
	const float Alpha = FMath::Sin(PulseTime * PulseSpeed * UE_TWO_PI) * 0.5f + 0.5f;
	ApplyIconTint(FMath::Lerp(FLinearColor::White, PulseColor, Alpha * PulseIntensity));
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
	CurrentState  = InState;
	bPulseActive  = false;
	PulseTime     = 0.f;

	switch (InState)
	{
	case ESkillTreeNodeState::Ineligible:
		if (Button_Node) Button_Node->SetIsEnabled(false);
		ApplyIconTint(IneligibleTint);
		break;

	case ESkillTreeNodeState::Available:
		if (Button_Node) Button_Node->SetIsEnabled(true);
		ApplyIconTint(FLinearColor::White);
		bPulseActive = true;     // NativeTick drives the animation
		break;

	case ESkillTreeNodeState::Pending:
		if (Button_Node) Button_Node->SetIsEnabled(true);
		ApplyIconTint(PendingTint);
		break;

	case ESkillTreeNodeState::Allocated:
		if (Button_Node) Button_Node->SetIsEnabled(false);
		ApplyIconTint(AllocatedTint);
		break;
	}

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

void UFP_SkillTreeNode::NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseEnter(InGeometry, InMouseEvent);
	OnNodeHoverChanged.Broadcast(this, true);
}

void UFP_SkillTreeNode::NativeOnMouseLeave(const FPointerEvent& InMouseEvent)
{
	Super::NativeOnMouseLeave(InMouseEvent);
	OnNodeHoverChanged.Broadcast(this, false);
}

void UFP_SkillTreeNode::ApplyIconTint(const FLinearColor& Tint) const
{
	if (Image_NodeIcon)
		Image_NodeIcon->SetColorAndOpacity(Tint);
}
