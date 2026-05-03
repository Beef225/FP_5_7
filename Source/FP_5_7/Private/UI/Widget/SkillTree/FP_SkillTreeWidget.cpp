// Copyright JG

#include "UI/Widget/SkillTree/FP_SkillTreeWidget.h"
#include "UI/Widget/SkillTree/FP_SkillTreeNode.h"
#include "SkillTree/FP_SkillTreeNodeData.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Rendering/DrawElements.h"

void UFP_SkillTreeWidget::NativeConstruct()
{
	Super::NativeConstruct();
	PopulateTree();
}

// ---------------------------------------------------------------------------
// Population
// ---------------------------------------------------------------------------

TSubclassOf<UFP_SkillTreeNode> UFP_SkillTreeWidget::ClassForType(ESkillTreeNodeType Type) const
{
	switch (Type)
	{
	case ESkillTreeNodeType::Major:   return NodeWidgetClass_Major;
	case ESkillTreeNodeType::Mastery: return NodeWidgetClass_Mastery;
	default:                          return NodeWidgetClass_Minor;
	}
}


void UFP_SkillTreeWidget::PopulateTree()
{
	const bool bAnyClass = NodeWidgetClass_Minor || NodeWidgetClass_Major || NodeWidgetClass_Mastery;
	if (!Canvas_Nodes)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillTreeWidget [%s]: Canvas_Nodes is not bound"), *GetName());
		return;
	}
	if (!bAnyClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillTreeWidget [%s]: no NodeWidgetClass set (Minor/Major/Mastery)"), *GetName());
		return;
	}

	UE_LOG(LogTemp, Log, TEXT("SkillTreeWidget [%s]: PopulateTree — %d data assets"), *GetName(), NodeDataAssets.Num());

	Canvas_Nodes->ClearChildren();
	SpawnedNodes.Empty();
	NodeDataList.Empty();
	ConnectionPairs.Empty();
	PendingNodeTags.Empty();

	for (const TObjectPtr<UFP_SkillTreeNodeData>& Data : NodeDataAssets)
	{
		if (!Data) continue;
		NodeDataList.Add(Data.Get());
		SpawnNodeWidget(Data.Get());
	}

	// Pre-compute unique connection pairs so NativePaint doesn't sort every frame.
	// Each edge is stored with the lexicographically smaller tag first.
	TSet<TTuple<FString, FString>> Seen;
	for (const TObjectPtr<const UFP_SkillTreeNodeData>& Data : NodeDataList)
	{
		if (!Data) continue;
		for (const FGameplayTag& Neighbour : Data->ConnectedNodeTags)
		{
			const FString A = Data->NodeTag.ToString();
			const FString B = Neighbour.ToString();
			const TTuple<FString, FString> Key = A < B
				? MakeTuple(A, B)
				: MakeTuple(B, A);

			if (Seen.Contains(Key)) continue;
			Seen.Add(Key);
			ConnectionPairs.Add(MakeTuple(Data->NodeTag, Neighbour));
		}
	}

	RecomputeAllStates();
}

void UFP_SkillTreeWidget::SpawnNodeWidget(const UFP_SkillTreeNodeData* Data)
{
	TSubclassOf<UFP_SkillTreeNode> WidgetClass = ClassForType(Data->NodeType);
	if (!WidgetClass)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillTreeWidget: no widget class for node type %d (tag: %s)"),
			(int32)Data->NodeType, *Data->NodeTag.ToString());
		return;
	}

	UFP_SkillTreeNode* NodeWidget = CreateWidget<UFP_SkillTreeNode>(GetOwningPlayer(), WidgetClass);
	if (!NodeWidget)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillTreeWidget: CreateWidget failed for %s"), *Data->NodeTag.ToString());
		return;
	}

	NodeWidget->Populate(Data, ComputeState(Data));
	NodeWidget->OnNodeClicked.AddUObject(this, &UFP_SkillTreeWidget::HandleNodeClicked);
	NodeWidget->OnNodeHoverChanged.AddUObject(this, &UFP_SkillTreeWidget::HandleNodeHoverChanged);

	UCanvasPanelSlot* NodeSlot = Canvas_Nodes->AddChildToCanvas(NodeWidget);
	if (NodeSlot)
	{
		// Read the half-size from the node widget's own SizeBox_Root override dimensions.
		// Each BP subclass sets its SizeBox_Root width/height to match its visual design.
		const FVector2D HalfSize = NodeWidget->GetDesiredHalfSize();
		const FVector2D Centre   = Data->NodePosition * PositionScale;
		NodeSlot->SetPosition(Centre - HalfSize);
		NodeSlot->SetSize(HalfSize * 2.f);
	}

	SpawnedNodes.Add(Data->NodeTag, NodeWidget);
}

// ---------------------------------------------------------------------------
// State
// ---------------------------------------------------------------------------

ESkillTreeNodeState UFP_SkillTreeWidget::ComputeState(const UFP_SkillTreeNodeData* Data) const
{
	if (!Data) return ESkillTreeNodeState::Ineligible;

	if (AllocatedNodeTags.HasTag(Data->NodeTag))
		return ESkillTreeNodeState::Allocated;

	if (PendingNodeTags.Contains(Data->NodeTag))
		return ESkillTreeNodeState::Pending;

	if (Data->bIsStarterNode)
		return ESkillTreeNodeState::Available;

	for (const FGameplayTag& Neighbour : Data->ConnectedNodeTags)
	{
		if (AllocatedNodeTags.HasTag(Neighbour) || PendingNodeTags.Contains(Neighbour))
			return ESkillTreeNodeState::Available;
	}

	return ESkillTreeNodeState::Ineligible;
}

void UFP_SkillTreeWidget::RecomputeAllStates()
{
	for (const TObjectPtr<const UFP_SkillTreeNodeData>& Data : NodeDataList)
	{
		if (!Data) continue;
		if (TObjectPtr<UFP_SkillTreeNode>* Widget = SpawnedNodes.Find(Data->NodeTag))
		{
			if (*Widget)
				(*Widget)->SetState(ComputeState(Data.Get()));
		}
	}

	Invalidate(EInvalidateWidgetReason::Paint);
}

// ---------------------------------------------------------------------------
// Interaction
// ---------------------------------------------------------------------------

void UFP_SkillTreeWidget::HandleNodeClicked(UFP_SkillTreeNode* Node)
{
	if (!Node) return;
	const FGameplayTag Tag = Node->GetNodeTag();

	// UFP_SkillTreeNode already toggled its own state (Available→Pending / Pending→Available).
	// Mirror that in our pending list.
	if (Node->GetCurrentState() == ESkillTreeNodeState::Pending)
		PendingNodeTags.AddUnique(Tag);
	else
		PendingNodeTags.Remove(Tag);

	// Recompute neighbours — a newly-pending node unlocks its neighbours.
	RecomputeAllStates();
	const int32 Count = PendingNodeTags.Num();
	OnPendingCountChanged(Count);
	OnPendingCountChangedDelegate.Broadcast(Count);
}

void UFP_SkillTreeWidget::HandleNodeHoverChanged(UFP_SkillTreeNode* Node, bool bEntered)
{
	OnNodeHoverChangedDelegate.Broadcast(Node, bEntered);
}

void UFP_SkillTreeWidget::LoadAllocatedState(const FGameplayTagContainer& InAllocatedNodes)
{
	AllocatedNodeTags = InAllocatedNodes;
	PendingNodeTags.Empty();
	RecomputeAllStates();
	OnPendingCountChanged(0);
}

void UFP_SkillTreeWidget::CommitPendingNodes()
{
	if (PendingNodeTags.IsEmpty()) return;

	APlayerController* PC = GetOwningPlayer();
	TArray<FGameplayTag> Committed;

	for (const FGameplayTag& Tag : PendingNodeTags)
	{
		for (const TObjectPtr<const UFP_SkillTreeNodeData>& Data : NodeDataList)
		{
			if (Data && Data->NodeTag == Tag)
			{
				const_cast<UFP_SkillTreeNodeData*>(Data.Get())->AllocateEffects(PC);
				AllocatedNodeTags.AddTag(Tag);
				Committed.Add(Tag);
				break;
			}
		}
	}

	PendingNodeTags.Empty();
	RecomputeAllStates();
	OnPendingCountChanged(0);

	if (!Committed.IsEmpty())
		OnNodesCommitted(Committed);
}

void UFP_SkillTreeWidget::CancelPendingNodes()
{
	if (PendingNodeTags.IsEmpty()) return;

	PendingNodeTags.Empty();
	RecomputeAllStates();
	OnPendingCountChanged(0);
}

// ---------------------------------------------------------------------------
// Connection line rendering
// ---------------------------------------------------------------------------

int32 UFP_SkillTreeWidget::NativePaint(
	const FPaintArgs& Args,
	const FGeometry& AllottedGeometry,
	const FSlateRect& MyCullingRect,
	FSlateWindowElementList& OutDrawElements,
	int32 LayerId,
	const FWidgetStyle& InWidgetStyle,
	bool bParentEnabled) const
{
	// Draw lines BELOW nodes — call Super after so nodes paint on top.
	// (If you want lines on top, call Super first and draw after.)

	// Build a tag→position lookup from data assets.
	// NodePosition is already the canvas centre in data-asset space.
	TMap<FGameplayTag, FVector2D> PosMap;
	PosMap.Reserve(NodeDataList.Num());
	for (const TObjectPtr<const UFP_SkillTreeNodeData>& Data : NodeDataList)
	{
		if (Data)
			PosMap.Add(Data->NodeTag, Data->NodePosition * PositionScale);
	}

	for (const TTuple<FGameplayTag, FGameplayTag>& Pair : ConnectionPairs)
	{
		const FVector2D* StartPtr = PosMap.Find(Pair.Get<0>());
		const FVector2D* EndPtr   = PosMap.Find(Pair.Get<1>());
		if (!StartPtr || !EndPtr) continue;

		// Pick colour based on state of both endpoints
		const bool bAActive = AllocatedNodeTags.HasTag(Pair.Get<0>()) || PendingNodeTags.Contains(Pair.Get<0>());
		const bool bBActive = AllocatedNodeTags.HasTag(Pair.Get<1>()) || PendingNodeTags.Contains(Pair.Get<1>());

		FLinearColor LineColor;
		if (bAActive && bBActive)
			LineColor = LineColor_Active;
		else if (bAActive || bBActive)
			LineColor = LineColor_Partial;
		else
			LineColor = LineColor_Inactive;

		const TArray<FVector2D> Points = { *StartPtr, *EndPtr };
		FSlateDrawElement::MakeLines(
			OutDrawElements,
			LayerId,
			AllottedGeometry.ToPaintGeometry(),
			Points,
			ESlateDrawEffect::None,
			LineColor,
			true,
			LineThickness);
	}

	++LayerId;
	return Super::NativePaint(Args, AllottedGeometry, MyCullingRect, OutDrawElements,
		LayerId, InWidgetStyle, bParentEnabled);
}

