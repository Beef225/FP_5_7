// Copyright JG

#include "UI/Widget/SkillTree/FP_SkillTreeWidget.h"
#include "UI/Widget/SkillTree/FP_SkillTreeNode.h"
#include "UI/Widget/SkillTree/FP_SkillTreeLines.h"
#include "SkillTree/FP_SkillTreeNodeData.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

void UFP_SkillTreeWidget::NativePreConstruct()
{
	Super::NativePreConstruct();
	PushLineStyle();
}

void UFP_SkillTreeWidget::NativeConstruct()
{
	Super::NativeConstruct();
	PushLineStyle();
	PopulateTree();
}

// ---------------------------------------------------------------------------
// Line style
// ---------------------------------------------------------------------------

void UFP_SkillTreeWidget::PushLineStyle()
{
	if (Canvas_Lines)
		Canvas_Lines->SetStyle(LineThickness, LineColor_Active, LineColor_Partial, LineColor_Inactive);
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
	CachedPosMap.Empty();
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

	// Build position map and push everything to the line layer
	CachedPosMap.Reserve(NodeDataList.Num());
	for (const TObjectPtr<const UFP_SkillTreeNodeData>& Data : NodeDataList)
	{
		if (Data)
			CachedPosMap.Add(Data->NodeTag, Data->NodePosition * PositionScale);
	}

	if (Canvas_Lines)
		Canvas_Lines->SetConnectionData(ConnectionPairs, CachedPosMap);

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
		NodeSlot->SetZOrder(1);
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

	if (Canvas_Lines)
		Canvas_Lines->SetState(AllocatedNodeTags, PendingNodeTags);
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
	UE_LOG(LogTemp, Log, TEXT("SkillTree CommitPendingNodes — %d nodes pending, PC: %s"),
		PendingNodeTags.Num(), PC ? *PC->GetName() : TEXT("null"));
	TArray<FGameplayTag> Committed;

	for (const FGameplayTag& Tag : PendingNodeTags)
	{
		for (const TObjectPtr<const UFP_SkillTreeNodeData>& Data : NodeDataList)
		{
			if (Data && Data->NodeTag == Tag)
			{
				UE_LOG(LogTemp, Log, TEXT("SkillTree CommitPendingNodes — allocating node [%s] with %d effects"),
					*Tag.ToString(), Data->Effects.Num());
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


