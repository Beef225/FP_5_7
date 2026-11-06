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

	Canvas_Nodes->ClearChildren();
	SpawnedNodes.Empty();
	NodeDataList.Empty();
	ConnectionPairs.Empty();
	CachedPosMap.Empty();
	PendingNodeTags.Empty();
	PendingRemovalTags.Empty();
	bRespecMode = false;
	Adjacency.Empty();
	StarterTags.Empty();

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
			Adjacency.FindOrAdd(Data->NodeTag).AddUnique(Neighbour);
			Adjacency.FindOrAdd(Neighbour).AddUnique(Data->NodeTag);
		}
		if (Data->bIsStarterNode)
			StarterTags.Add(Data->NodeTag);
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
		const FVector2D HalfSize = NodeWidget->GetDesiredHalfSize();
		const FVector2D Centre   = Data->NodePosition * PositionScale;
		const FVector2D SlotPos  = Centre - HalfSize;
		const FVector2D SlotSize = HalfSize * 2.f;
		NodeSlot->SetPosition(SlotPos);
		NodeSlot->SetSize(SlotSize);
		NodeSlot->SetAutoSize(false);
		NodeSlot->SetZOrder(1);
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillTreeWidget: AddChildToCanvas returned null slot for %s"), *Data->NodeTag.ToString());
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
	if (!bRespecMode)
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
		return;
	}

	// Respec mode: only allocated nodes react.
	const TSet<FGameplayTag> Remaining    = GetRemainingAllocated();
	const TSet<FGameplayTag> ReachableNow = GetReachableFromStarters(Remaining);

	for (const TObjectPtr<const UFP_SkillTreeNodeData>& Data : NodeDataList)
	{
		if (!Data) continue;
		TObjectPtr<UFP_SkillTreeNode>* Widget = SpawnedNodes.Find(Data->NodeTag);
		if (!Widget || !*Widget) continue;

		const FGameplayTag& Tag = Data->NodeTag;
		ESkillTreeNodeState State = ESkillTreeNodeState::Ineligible;
		if (PendingRemovalTags.Contains(Tag))
			State = ESkillTreeNodeState::PendingRemoval;
		else if (AllocatedNodeTags.HasTagExact(Tag))
			State = CanRemove(Tag, Remaining, ReachableNow)
				? ESkillTreeNodeState::Removable
				: ESkillTreeNodeState::Allocated;

		(*Widget)->SetState(State);
	}

	if (Canvas_Lines)
	{
		// Lines to nodes marked for removal drop back to inactive.
		FGameplayTagContainer RemainingContainer;
		for (const FGameplayTag& Tag : Remaining)
			RemainingContainer.AddTagFast(Tag);
		Canvas_Lines->SetState(RemainingContainer, TArray<FGameplayTag>());
	}
}

void UFP_SkillTreeWidget::BroadcastPendingCount()
{
	const int32 Count = GetPendingChangeCount();
	OnPendingCountChanged(Count);
	OnPendingCountChangedDelegate.Broadcast(Count);
}

// ---------------------------------------------------------------------------
// Respec
// ---------------------------------------------------------------------------

TSet<FGameplayTag> UFP_SkillTreeWidget::GetRemainingAllocated() const
{
	TSet<FGameplayTag> Remaining;
	for (const FGameplayTag& Tag : AllocatedNodeTags)
	{
		if (!PendingRemovalTags.Contains(Tag))
			Remaining.Add(Tag);
	}
	return Remaining;
}

TSet<FGameplayTag> UFP_SkillTreeWidget::GetReachableFromStarters(const TSet<FGameplayTag>& Set) const
{
	TSet<FGameplayTag>   Visited;
	TArray<FGameplayTag> Stack;
	for (const FGameplayTag& Starter : StarterTags)
	{
		if (Set.Contains(Starter) && !Visited.Contains(Starter))
		{
			Visited.Add(Starter);
			Stack.Add(Starter);
		}
	}
	while (!Stack.IsEmpty())
	{
		const FGameplayTag Current = Stack.Pop(EAllowShrinking::No);
		if (const TArray<FGameplayTag>* Neighbours = Adjacency.Find(Current))
		{
			for (const FGameplayTag& N : *Neighbours)
			{
				if (Set.Contains(N) && !Visited.Contains(N))
				{
					Visited.Add(N);
					Stack.Add(N);
				}
			}
		}
	}
	return Visited;
}

bool UFP_SkillTreeWidget::CanRemove(const FGameplayTag& Tag, const TSet<FGameplayTag>& Remaining,
                                    const TSet<FGameplayTag>& ReachableNow) const
{
	// Removing Tag may only drop Tag itself from the connected set — losing anything else
	// would strand it. Comparing against the current reachable set (rather than requiring
	// everything to be reachable) keeps nodes that are already disconnected (older saves,
	// debug allocations) refundable instead of locking the whole tree.
	TSet<FGameplayTag> Without = Remaining;
	Without.Remove(Tag);
	const int32 ExpectedLoss = ReachableNow.Contains(Tag) ? 1 : 0;
	return ReachableNow.Num() - GetReachableFromStarters(Without).Num() <= ExpectedLoss;
}

bool UFP_SkillTreeWidget::CanRestore(const FGameplayTag& Tag, const TSet<FGameplayTag>& Remaining) const
{
	if (StarterTags.Contains(Tag)) return true;
	if (const TArray<FGameplayTag>* Neighbours = Adjacency.Find(Tag))
	{
		for (const FGameplayTag& N : *Neighbours)
		{
			if (Remaining.Contains(N)) return true;
		}
	}
	return false;
}

void UFP_SkillTreeWidget::SetRespecMode(bool bEnable)
{
	if (bRespecMode == bEnable) return;

	if (bEnable)
		PendingNodeTags.Empty();   // can't mix adds and refunds in one confirm
	else
		PendingRemovalTags.Empty();

	bRespecMode = bEnable;
	RecomputeAllStates();
	OnRespecModeChanged(bRespecMode);
	BroadcastPendingCount();
}

void UFP_SkillTreeWidget::CommitPendingRemovals()
{
	if (!bRespecMode) return;

	APlayerController* PC = GetOwningPlayer();
	TArray<FGameplayTag> Refunded;

	for (const FGameplayTag& Tag : PendingRemovalTags)
	{
		for (const TObjectPtr<const UFP_SkillTreeNodeData>& Data : NodeDataList)
		{
			if (Data && Data->NodeTag == Tag)
			{
				const_cast<UFP_SkillTreeNodeData*>(Data.Get())->DeallocateEffects(PC);
				break;
			}
		}
		AllocatedNodeTags.RemoveTag(Tag);
		Refunded.Add(Tag);
	}

	PendingRemovalTags.Empty();
	bRespecMode = false;
	RecomputeAllStates();
	OnRespecModeChanged(false);
	BroadcastPendingCount();

	if (!Refunded.IsEmpty())
		OnNodesRefunded(Refunded);
}

// ---------------------------------------------------------------------------
// Interaction
// ---------------------------------------------------------------------------

void UFP_SkillTreeWidget::HandleNodeClicked(UFP_SkillTreeNode* Node)
{
	if (!Node) return;
	const FGameplayTag Tag = Node->GetNodeTag();

	if (bRespecMode)
	{
		// Node toggled Removable→PendingRemoval or back.
		if (Node->GetCurrentState() == ESkillTreeNodeState::PendingRemoval)
		{
			PendingRemovalTags.AddUnique(Tag);
		}
		else
		{
			// Keeping it again is only valid while something still connects it.
			TSet<FGameplayTag> Remaining = GetRemainingAllocated();
			Remaining.Remove(Tag);
			if (!CanRestore(Tag, Remaining))
			{
				Node->SetState(ESkillTreeNodeState::PendingRemoval);
				return;
			}
			PendingRemovalTags.Remove(Tag);
		}
		RecomputeAllStates();
		BroadcastPendingCount();
		return;
	}

	// UFP_SkillTreeNode already toggled its own state (Available→Pending / Pending→Available).
	// Mirror that in our pending list.
	if (Node->GetCurrentState() == ESkillTreeNodeState::Pending)
	{
		// Out of points: undo the node's toggle and refuse.
		if (PointBudget != INDEX_NONE && PendingNodeTags.Num() >= PointBudget)
		{
			Node->SetState(ESkillTreeNodeState::Available);
			OnNotEnoughPoints(Tag);
			return;
		}
		PendingNodeTags.AddUnique(Tag);
	}
	else
		PendingNodeTags.Remove(Tag);

	// Recompute neighbours — a newly-pending node unlocks its neighbours.
	RecomputeAllStates();
	BroadcastPendingCount();
}

void UFP_SkillTreeWidget::HandleNodeHoverChanged(UFP_SkillTreeNode* Node, bool bEntered)
{
	OnNodeHoverChangedDelegate.Broadcast(Node, bEntered);
}

void UFP_SkillTreeWidget::LoadAllocatedState(const FGameplayTagContainer& InAllocatedNodes)
{
	AllocatedNodeTags = InAllocatedNodes;
	PendingNodeTags.Empty();
	PendingRemovalTags.Empty();
	const bool bWasRespec = bRespecMode;
	bRespecMode = false;
	RecomputeAllStates();
	if (bWasRespec)
		OnRespecModeChanged(false);
	BroadcastPendingCount();
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


