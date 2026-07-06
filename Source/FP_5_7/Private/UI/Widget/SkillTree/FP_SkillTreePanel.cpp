// Copyright JG

#include "UI/Widget/SkillTree/FP_SkillTreePanel.h"
#include "UI/Widget/SkillTree/FP_SkillTreeNode.h"
#include "UI/Widget/SkillTree/FP_SkillTreeNodeDescription.h"
#include "Player/FP_PlayerState.h"
#include "Components/NamedSlot.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"

// ---------------------------------------------------------------------------
// Construct
// ---------------------------------------------------------------------------

void UFP_SkillTreePanel::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_Confirm)
	{
		Button_Confirm->OnClicked.AddDynamic(this, &UFP_SkillTreePanel::HandleConfirmClicked);
		Button_Confirm->SetIsEnabled(false);
	}
	if (Button_Cancel)
	{
		Button_Cancel->OnClicked.AddDynamic(this, &UFP_SkillTreePanel::HandleCancelClicked);
		Button_Cancel->SetIsEnabled(false);
	}
	if (Canvas_Popup)
		Canvas_Popup->SetVisibility(ESlateVisibility::HitTestInvisible);
	if (Text_PendingCount)
		Text_PendingCount->SetVisibility(ESlateVisibility::Collapsed);
}

// ---------------------------------------------------------------------------
// Public API
// ---------------------------------------------------------------------------

void UFP_SkillTreePanel::OpenForSkill(TSubclassOf<UFP_SkillTreeWidget> TreeWidgetClass,
                                       const FGameplayTagContainer&      AllocatedNodes,
                                       const FText&                      SkillName,
                                       FGameplayTag                      SkillTag)
{
	if (!TreeWidgetClass || !Slot_Tree)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillTreePanel: OpenForSkill — null class or unbound Slot_Tree"));
		return;
	}

	UnwireActiveTree();

	CachedSkillTag = SkillTag;

	ActiveTree = CreateWidget<UFP_SkillTreeWidget>(GetOwningPlayer(), TreeWidgetClass);
	if (!ActiveTree) return;

	// SetContent fires NativeConstruct on the tree widget (PopulateTree runs there).
	Slot_Tree->SetContent(ActiveTree);

	// Belt-and-suspenders: call PopulateTree explicitly in case NativeConstruct timing
	// is deferred (e.g. widget not yet in the live viewport when SetContent is called).
	if (ActiveTree->GetSpawnedNodeCount() == 0)
	{
		ActiveTree->PopulateTree();
	}

	ActiveTree->LoadAllocatedState(AllocatedNodes);

	WireActiveTree();

	if (Text_SkillName)
		Text_SkillName->SetText(SkillName);

	OnSkillTreeOpened(SkillName);
}

void UFP_SkillTreePanel::Close()
{
	if (ActiveTree)
		ActiveTree->CancelPendingNodes();

	HideNodePopup();
	OnSkillTreeClosed();
}

// ---------------------------------------------------------------------------
// Wiring
// ---------------------------------------------------------------------------

void UFP_SkillTreePanel::WireActiveTree()
{
	if (!ActiveTree) return;

	ActiveTree->OnPendingCountChangedDelegate.AddDynamic(
		this, &UFP_SkillTreePanel::HandlePendingCountChanged);
	ActiveTree->OnNodeHoverChangedDelegate.AddUObject(
		this, &UFP_SkillTreePanel::HandleNodeHoverChanged);

	if (Button_Cancel) Button_Cancel->SetIsEnabled(true);
	UpdatePointsDisplay(0);
}

void UFP_SkillTreePanel::UnwireActiveTree()
{
	if (!ActiveTree) return;

	ActiveTree->OnPendingCountChangedDelegate.RemoveDynamic(
		this, &UFP_SkillTreePanel::HandlePendingCountChanged);
	ActiveTree->OnNodeHoverChangedDelegate.RemoveAll(this);

	if (Slot_Tree) Slot_Tree->SetContent(nullptr);
	ActiveTree = nullptr;
}

// ---------------------------------------------------------------------------
// Points display
// ---------------------------------------------------------------------------

void UFP_SkillTreePanel::UpdatePointsDisplay(int32 PendingCount)
{
	const AFP_PlayerState* PS = GetOwningPlayerState<AFP_PlayerState>();
	const int32 Available = PS ? PS->GetSkillPoints(CachedSkillTag) : 0;
	const int32 Spent     = ActiveTree ? ActiveTree->GetAllocatedNodeTags().Num() : 0;
	const int32 Remaining = Available - PendingCount;

	if (Text_PointsSpent)
		Text_PointsSpent->SetText(FText::AsNumber(Spent));

	if (Text_PointsAvailable)
		Text_PointsAvailable->SetText(FText::AsNumber(FMath::Max(0, Remaining)));

	if (Text_PendingCount)
	{
		if (PendingCount > 0)
		{
			Text_PendingCount->SetText(FText::FromString(FString::Printf(TEXT("+%d"), PendingCount)));
			Text_PendingCount->SetVisibility(ESlateVisibility::HitTestInvisible);
		}
		else
		{
			Text_PendingCount->SetVisibility(ESlateVisibility::Collapsed);
		}
	}

	if (Button_Confirm)
		Button_Confirm->SetIsEnabled(PendingCount > 0 && Remaining >= 0);
}

// ---------------------------------------------------------------------------
// Button handlers
// ---------------------------------------------------------------------------

void UFP_SkillTreePanel::HandleConfirmClicked()
{
	if (!ActiveTree) return;

	const int32            PointsToSpend = ActiveTree->GetPendingCount();
	const FGameplayTag     TreeTag       = ActiveTree->GetTreeTag();

	ActiveTree->CommitPendingNodes();
	const FGameplayTagContainer Allocated = ActiveTree->GetAllocatedNodeTags();

	if (PointsToSpend > 0)
		if (AFP_PlayerState* PS = GetOwningPlayerState<AFP_PlayerState>())
			PS->SpendSkillPassivePoints(CachedSkillTag, PointsToSpend);

	if (AFP_PlayerState* PS = GetOwningPlayerState<AFP_PlayerState>())
		PS->SetSkillTreeAllocatedNodes(TreeTag, Allocated);

	OnPanelCommitted.Broadcast(TreeTag, Allocated);
	UpdatePointsDisplay(0);
	Close();
}

void UFP_SkillTreePanel::HandleCancelClicked()
{
	Close();
}

void UFP_SkillTreePanel::HandlePendingCountChanged(int32 NewCount)
{
	UpdatePointsDisplay(NewCount);
}

// ---------------------------------------------------------------------------
// Node popup
// ---------------------------------------------------------------------------

void UFP_SkillTreePanel::HandleNodeHoverChanged(UFP_SkillTreeNode* Node, bool bEntered)
{
	if (bEntered) ShowNodePopup(Node);
	else          HideNodePopup();
}

void UFP_SkillTreePanel::ShowNodePopup(UFP_SkillTreeNode* Node)
{
	if (!Canvas_Popup || !NodeDescriptionClass || !Node) return;

	const UFP_SkillTreeNodeData* Data = Node->GetNodeData();
	if (!Data) return;

	if (!NodePopup)
	{
		NodePopup = CreateWidget<UFP_SkillTreeNodeDescription>(GetOwningPlayer(), NodeDescriptionClass);
		if (!NodePopup) return;
		if (UCanvasPanelSlot* PopupSlot = Canvas_Popup->AddChildToCanvas(NodePopup))
			PopupSlot->SetAutoSize(true);
	}

	NodePopup->Populate(Data);
	NodePopup->SetVisibility(ESlateVisibility::HitTestInvisible);
	NodePopup->ForceLayoutPrepass();

	const FGeometry& PanelGeo    = GetCachedGeometry();
	const FGeometry  NodeGeo     = Node->GetCachedGeometry();
	const FVector2D  NodeAbsTL   = NodeGeo.GetAbsolutePosition();
	const FVector2D  NodeAbsSize = NodeGeo.GetAbsoluteSize();
	const FVector2D  NodeLocalTL = PanelGeo.AbsoluteToLocal(NodeAbsTL);
	const FVector2D  NodeLocalBR = PanelGeo.AbsoluteToLocal(NodeAbsTL + NodeAbsSize);
	const FVector2D  PopupSize   = NodePopup->GetDesiredSize();
	const FVector2D  PanelSize   = PanelGeo.GetLocalSize();

	float PX = (NodeLocalTL.X + NodeLocalBR.X) * 0.5f - PopupSize.X * 0.5f;
	float PY = NodeLocalTL.Y - PopupSize.Y;
	if (PY < 0.f) PY = NodeLocalBR.Y;
	PX = FMath::Clamp(PX, 0.f, FMath::Max(0.f, PanelSize.X - PopupSize.X));

	if (UCanvasPanelSlot* PopupSlot = Cast<UCanvasPanelSlot>(NodePopup->Slot))
		PopupSlot->SetPosition(FVector2D(PX, PY));
}

void UFP_SkillTreePanel::HideNodePopup()
{
	if (NodePopup)
		NodePopup->SetVisibility(ESlateVisibility::Collapsed);
}
