// Copyright JG

#include "UI/Widget/SkillTree/FP_SkillTreeContainer.h"
#include "UI/Widget/SkillTree/FP_SkillTreeWidget.h"
#include "UI/Widget/SkillTree/FP_SkillTreeNode.h"
#include "UI/Widget/SkillTree/FP_SkillTreeNodeDescription.h"
#include "Player/FP_PlayerState.h"
#include "Components/WidgetSwitcher.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
#include "Blueprint/WidgetTree.h"
#include "InputCoreTypes.h"

// ---------------------------------------------------------------------------
// Construct
// ---------------------------------------------------------------------------

void UFP_SkillTreeContainer::NativeConstruct()
{
	Super::NativeConstruct();

	if (Button_Confirm)
		Button_Confirm->OnClicked.AddDynamic(this, &UFP_SkillTreeContainer::HandleConfirmClicked);
	if (Button_Cancel)
		Button_Cancel->OnClicked.AddDynamic(this, &UFP_SkillTreeContainer::HandleCancelClicked);
	if (Button_Respec)
		Button_Respec->OnClicked.AddDynamic(this, &UFP_SkillTreeContainer::HandleRespecClicked);

	if (Button_Confirm) Button_Confirm->SetIsEnabled(false);
	if (Button_Cancel)  Button_Cancel->SetIsEnabled(false);

	if (Canvas_Popup)
		Canvas_Popup->SetVisibility(ESlateVisibility::HitTestInvisible);

	if (!Switcher_Trees)
	{
		UE_LOG(LogTemp, Warning, TEXT("SkillTreeContainer [%s]: Switcher_Trees not bound"), *GetName());
		return;
	}
	Switcher_Trees->SetVisibility(ESlateVisibility::SelfHitTestInvisible);

	if (AFP_PlayerState* PS = GetOwningPlayerState<AFP_PlayerState>())
	{
		PS->OnPassivePointsChangedDelegate.AddUObject(
			this, &UFP_SkillTreeContainer::HandlePassivePointsChanged);
		PS->OnSkillStateLoaded.AddUObject(
			this, &UFP_SkillTreeContainer::HandleSkillStateLoaded);
	}

	SpawnTreeWidgets();
	LoadAllFromPlayerState();

	// Wire the initially-active tree and seed points display
	WireActiveTree();
}

// ---------------------------------------------------------------------------
// Tree spawning
// ---------------------------------------------------------------------------

void UFP_SkillTreeContainer::SpawnTreeWidgets()
{
	if (!Switcher_Trees) return;

	TreeWidgets.Empty();
	ViewStates.Empty();

	if (!TreeWidgetClasses.IsEmpty())
	{
		// --- Programmatic mode: spawn from TreeWidgetClasses array ---
		for (int32 i = 0; i < TreeWidgetClasses.Num(); ++i)
		{
			UOverlay* SlotOverlay = WidgetTree->ConstructWidget<UOverlay>(UOverlay::StaticClass());
			SlotOverlay->SetVisibility(ESlateVisibility::SelfHitTestInvisible);
			Switcher_Trees->AddChild(SlotOverlay);

			UFP_SkillTreeWidget* TreeWidget = nullptr;
			if (TreeWidgetClasses[i])
			{
				TreeWidget = CreateWidget<UFP_SkillTreeWidget>(GetOwningPlayer(), TreeWidgetClasses[i]);
				if (TreeWidget)
				{
					if (UOverlaySlot* OSlot = SlotOverlay->AddChildToOverlay(TreeWidget))
					{
						OSlot->SetHorizontalAlignment(HAlign_Left);
						OSlot->SetVerticalAlignment(VAlign_Top);
						OSlot->SetPadding(FMargin(0.f));
					}
					TreeWidget->SetRenderTransformPivot(FVector2D(0.f, 0.f));
				}
			}
			TreeWidgets.Add(TreeWidget);
			ViewStates.Add(FTreeViewState{});
		}
	}
	else
	{
		// --- Designer mode: tree widgets placed manually in the BP ---
		// Walk the switcher's existing children and find UFP_SkillTreeWidget
		// instances (directly or one panel level deep).
		for (int32 i = 0; i < Switcher_Trees->GetChildrenCount(); ++i)
		{
			UWidget* SlotWidget = Switcher_Trees->GetChildAt(i);
			UFP_SkillTreeWidget* Found = Cast<UFP_SkillTreeWidget>(SlotWidget);
			if (!Found)
			{
				if (UPanelWidget* Panel = Cast<UPanelWidget>(SlotWidget))
				{
					for (int32 j = 0; j < Panel->GetChildrenCount(); ++j)
					{
						Found = Cast<UFP_SkillTreeWidget>(Panel->GetChildAt(j));
						if (Found) break;
					}
				}
			}
			if (Found)
				Found->SetRenderTransformPivot(FVector2D(0.f, 0.f));

			TreeWidgets.Add(Found);
			ViewStates.Add(FTreeViewState{});
		}
	}

	Switcher_Trees->SetActiveWidgetIndex(0);
	ActiveTreeIndex = 0;
}

// ---------------------------------------------------------------------------
// Tree switching
// ---------------------------------------------------------------------------

void UFP_SkillTreeContainer::SwitchToTree(int32 Index)
{
	const int32 TreeCount = Switcher_Trees ? Switcher_Trees->GetChildrenCount() : 0;
	if (Index < 0 || Index >= TreeCount || Index == ActiveTreeIndex) return;

	// Leaving a tree mid-respec discards its pending refunds.
	if (UFP_SkillTreeWidget* Old = GetActiveTreeWidget(); Old && Old->IsInRespecMode())
	{
		Old->SetRespecMode(false);
		OnRespecModeChanged(false);
	}

	UnwireTree(GetActiveTreeWidget());
	HideNodePopup();

	ActiveTreeIndex = Index;
	Switcher_Trees->SetActiveWidgetIndex(Index);

	WireActiveTree();

	// Restore the saved transform for this tree (or let NativeTick centre it)
	if (!ViewStates[Index].bNeedsCenter)
		ApplyTransform();

	OnTreeSwitched(Index);
}

UFP_SkillTreeWidget* UFP_SkillTreeContainer::GetActiveTreeWidget() const
{
	return TreeWidgets.IsValidIndex(ActiveTreeIndex) ? TreeWidgets[ActiveTreeIndex] : nullptr;
}

// ---------------------------------------------------------------------------
// Delegate wiring
// ---------------------------------------------------------------------------

void UFP_SkillTreeContainer::WireActiveTree()
{
	UFP_SkillTreeWidget* Active = GetActiveTreeWidget();
	if (!Active) return;

	Active->OnPendingCountChangedDelegate.AddDynamic(
		this, &UFP_SkillTreeContainer::HandlePendingCountChanged);
	Active->OnNodeHoverChangedDelegate.AddUObject(
		this, &UFP_SkillTreeContainer::HandleNodeHoverChanged);

	RefreshActivePoints();
}

void UFP_SkillTreeContainer::LoadAllFromPlayerState()
{
	const AFP_PlayerState* PS = GetOwningPlayerState<AFP_PlayerState>();
	if (!PS) return;

	for (UFP_SkillTreeWidget* Tree : TreeWidgets)
	{
		if (!Tree) continue;
		// NativeConstruct on the tree may not have run yet; LoadAllocatedState needs nodes spawned.
		if (Tree->GetSpawnedNodeCount() == 0)
			Tree->PopulateTree();
		Tree->LoadAllocatedState(PS->GetSkillTreeNodesForTree(Tree->GetTreeTag()));
	}
}

bool UFP_SkillTreeContainer::IgnoresPointCost() const
{
	const AFP_PlayerState* PS = GetOwningPlayerState<AFP_PlayerState>();
	return PS && PS->ShouldIgnoreSkillTreePointCost();
}

void UFP_SkillTreeContainer::RefreshActivePoints()
{
	UFP_SkillTreeWidget* Active = GetActiveTreeWidget();
	if (!Active) return;

	if (const AFP_PlayerState* PS = GetOwningPlayerState<AFP_PlayerState>())
		CachedAvailablePoints = PS->GetPassivePointsForTree(Active->GetTreeTag());

	Active->SetPointBudget(IgnoresPointCost() ? INDEX_NONE : FMath::Max(0, CachedAvailablePoints));
	HandlePendingCountChanged(Active->GetPendingChangeCount());
}

bool UFP_SkillTreeContainer::IsInRespecMode() const
{
	const UFP_SkillTreeWidget* Active = GetActiveTreeWidget();
	return Active && Active->IsInRespecMode();
}

void UFP_SkillTreeContainer::SetRespecMode(bool bEnable)
{
	UFP_SkillTreeWidget* Active = GetActiveTreeWidget();
	if (!Active || Active->IsInRespecMode() == bEnable) return;

	HideNodePopup();
	Active->SetRespecMode(bEnable);   // broadcasts the pending count → buttons + display refresh
	OnRespecModeChanged(bEnable);
}

void UFP_SkillTreeContainer::ToggleRespecMode()
{
	SetRespecMode(!IsInRespecMode());
}

void UFP_SkillTreeContainer::HandleRespecClicked()
{
	ToggleRespecMode();
}

void UFP_SkillTreeContainer::HandleSkillStateLoaded()
{
	LoadAllFromPlayerState();
	RefreshActivePoints();
}

void UFP_SkillTreeContainer::UnwireTree(UFP_SkillTreeWidget* Tree)
{
	if (!Tree) return;
	Tree->OnPendingCountChangedDelegate.RemoveDynamic(
		this, &UFP_SkillTreeContainer::HandlePendingCountChanged);
	Tree->OnNodeHoverChangedDelegate.RemoveAll(this);
}

void UFP_SkillTreeContainer::LoadAllocatedState(int32 TreeIndex, const FGameplayTagContainer& InAllocated)
{
	if (TreeWidgets.IsValidIndex(TreeIndex) && TreeWidgets[TreeIndex])
		TreeWidgets[TreeIndex]->LoadAllocatedState(InAllocated);
}

// ---------------------------------------------------------------------------
// View helpers
// ---------------------------------------------------------------------------

void UFP_SkillTreeContainer::ApplyTransform()
{
	UFP_SkillTreeWidget* Active = GetActiveTreeWidget();
	if (!Active || !ViewStates.IsValidIndex(ActiveTreeIndex)) return;

	const FTreeViewState& S = ViewStates[ActiveTreeIndex];
	FWidgetTransform T;
	T.Scale       = FVector2D(S.Zoom);
	T.Translation = S.Offset;
	Active->SetRenderTransform(T);
}

void UFP_SkillTreeContainer::ResetView()
{
	if (!ViewStates.IsValidIndex(ActiveTreeIndex)) return;
	ViewStates[ActiveTreeIndex].Zoom        = 1.f;
	ViewStates[ActiveTreeIndex].bNeedsCenter = true;
}

// ---------------------------------------------------------------------------
// NativeTick — centre active tree on first frame with valid geometry
// ---------------------------------------------------------------------------

void UFP_SkillTreeContainer::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);

	if (!ViewStates.IsValidIndex(ActiveTreeIndex)) return;
	FTreeViewState& S = ViewStates[ActiveTreeIndex];
	if (!S.bNeedsCenter) return;

	// Prefer the switcher's own geometry so the centering excludes the button bar
	FVector2D ViewportSize = Switcher_Trees
		? Switcher_Trees->GetCachedGeometry().GetLocalSize()
		: MyGeometry.GetLocalSize();

	if (ViewportSize.IsNearlyZero())
		ViewportSize = MyGeometry.GetLocalSize();
	if (ViewportSize.IsNearlyZero()) return;

	const FVector2D CanvasCentre = FVector2D(TreeCanvasSize * 0.5f);
	S.Offset = ViewportSize * 0.5f - CanvasCentre * S.Zoom;
	ApplyTransform();
	S.bNeedsCenter = false;
}

// ---------------------------------------------------------------------------
// Input — mouse wheel zoom
// ---------------------------------------------------------------------------

FReply UFP_SkillTreeContainer::NativeOnMouseWheel(
	const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (!ViewStates.IsValidIndex(ActiveTreeIndex)) return FReply::Unhandled();
	FTreeViewState& S = ViewStates[ActiveTreeIndex];

	const float NewZoom = FMath::Clamp(
		S.Zoom + InMouseEvent.GetWheelDelta() * ZoomStep, ZoomMin, ZoomMax);

	const FVector2D MouseLocal = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());
	const float ZoomRatio = NewZoom / S.Zoom;
	S.Offset = MouseLocal + (S.Offset - MouseLocal) * ZoomRatio;
	S.Zoom   = NewZoom;

	ApplyTransform();
	return FReply::Handled();
}

// ---------------------------------------------------------------------------
// Input — right-click pan
// ---------------------------------------------------------------------------

FReply UFP_SkillTreeContainer::NativeOnMouseButtonDown(
	const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		bIsPanning = true;
		if (TSharedPtr<SWidget> SafeWidget = GetCachedWidget())
			return FReply::Handled().CaptureMouse(SafeWidget.ToSharedRef());
		return FReply::Handled();
	}
	return Super::NativeOnMouseButtonDown(InGeometry, InMouseEvent);
}

FReply UFP_SkillTreeContainer::NativeOnMouseButtonUp(
	const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (InMouseEvent.GetEffectingButton() == EKeys::RightMouseButton)
	{
		bIsPanning = false;
		return FReply::Handled().ReleaseMouseCapture();
	}
	return Super::NativeOnMouseButtonUp(InGeometry, InMouseEvent);
}

FReply UFP_SkillTreeContainer::NativeOnMouseMove(
	const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	if (bIsPanning && ViewStates.IsValidIndex(ActiveTreeIndex))
	{
		ViewStates[ActiveTreeIndex].Offset += InMouseEvent.GetCursorDelta();
		ApplyTransform();
		return FReply::Handled();
	}
	return Super::NativeOnMouseMove(InGeometry, InMouseEvent);
}

// ---------------------------------------------------------------------------
// Button handlers
// ---------------------------------------------------------------------------

void UFP_SkillTreeContainer::HandlePendingCountChanged(int32 NewCount)
{
	const bool bHasPending = NewCount > 0;
	if (IsInRespecMode())
	{
		// Refunds never need points; Cancel stays live so respec can be exited with no changes.
		if (Button_Confirm) Button_Confirm->SetIsEnabled(bHasPending);
		if (Button_Cancel)  Button_Cancel->SetIsEnabled(true);
	}
	else
	{
		const bool bAffordable = IgnoresPointCost() || NewCount <= CachedAvailablePoints;
		if (Button_Confirm) Button_Confirm->SetIsEnabled(bHasPending && bAffordable);
		if (Button_Cancel)  Button_Cancel->SetIsEnabled(bHasPending);
	}
	UpdatePointsDisplay(NewCount);
}

void UFP_SkillTreeContainer::HandlePassivePointsChanged(FGameplayTag Tag, int32 NewPoints)
{
	RefreshActivePoints();
}

void UFP_SkillTreeContainer::UpdatePointsDisplay(int32 PendingCount)
{
	UFP_SkillTreeWidget* Active = GetActiveTreeWidget();
	if (!Active) return;

	const int32 Spent    = Active->GetAllocatedNodeTags().Num();
	const bool  bRespec  = Active->IsInRespecMode();

	if (Text_PointsSpent)
	{
		FText SpentText = FText::AsNumber(Spent);
		if (PendingCount > 0)
			SpentText = FText::FromString(bRespec
				? FString::Printf(TEXT("%d-%d"), Spent, PendingCount)
				: FString::Printf(TEXT("%d+%d"), Spent, PendingCount));
		Text_PointsSpent->SetText(SpentText);
	}

	// Available is derived (invested − allocated), so a refund only shows as a point once the
	// tree is back under the invested count — debug over-allocations never create points.
	if (Text_PointsAvailable)
	{
		const int32 Available = bRespec ? CachedAvailablePoints + PendingCount : CachedAvailablePoints - PendingCount;
		Text_PointsAvailable->SetText(FText::AsNumber(FMath::Max(0, Available)));
	}
}

void UFP_SkillTreeContainer::HandleConfirmClicked()
{
	UFP_SkillTreeWidget* Active = GetActiveTreeWidget();
	if (!Active) return;

	AFP_PlayerState* PS = GetOwningPlayerState<AFP_PlayerState>();
	if (!PS) return;

	// --- Respec: refund the marked nodes ---
	if (Active->IsInRespecMode())
	{
		if (Active->GetPendingRemovalCount() <= 0) return;

		Active->CommitPendingRemovals();   // deallocates effects, leaves respec mode
		PS->SetSkillTreeAllocatedNodes(Active->GetTreeTag(), Active->GetAllocatedNodeTags());
		OnRespecModeChanged(false);
		RefreshActivePoints();
		PS->VerifyPassiveTreePoints(Active->GetTreeTag());

		OnCommitClicked();
		return;
	}

	const int32 PointsToSpend = Active->GetPendingCount();
	if (PointsToSpend <= 0) return;

	// Re-check against the live pool (button state could be stale).
	if (!IgnoresPointCost() && PointsToSpend > PS->GetPassivePointsForTree(Active->GetTreeTag()))
	{
		RefreshActivePoints();
		return;
	}

	Active->CommitPendingNodes();

	// Points are derived (invested − allocated), so storing the allocation is the spend.
	// SaveActiveCharacter writes this map to the save record.
	PS->SetSkillTreeAllocatedNodes(Active->GetTreeTag(), Active->GetAllocatedNodeTags());
	RefreshActivePoints();
	PS->VerifyPassiveTreePoints(Active->GetTreeTag());

	OnCommitClicked();
}

void UFP_SkillTreeContainer::HandleCancelClicked()
{
	if (IsInRespecMode())
		SetRespecMode(false);   // discards pending refunds
	else if (UFP_SkillTreeWidget* Active = GetActiveTreeWidget())
		Active->CancelPendingNodes();
	OnCancelClicked();
}

// ---------------------------------------------------------------------------
// Node description popup
// ---------------------------------------------------------------------------

void UFP_SkillTreeContainer::HandleNodeHoverChanged(UFP_SkillTreeNode* Node, bool bEntered)
{
	if (bEntered) ShowNodePopup(Node);
	else          HideNodePopup();
}

void UFP_SkillTreeContainer::ShowNodePopup(UFP_SkillTreeNode* Node)
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

	const FGeometry NodeGeo        = Node->GetCachedGeometry();
	const FVector2D NodeAbsTopLeft = NodeGeo.GetAbsolutePosition();
	const FVector2D NodeAbsSize    = NodeGeo.GetAbsoluteSize();

	const FGeometry ContainerGeo  = GetCachedGeometry();
	const FVector2D ContainerSize = ContainerGeo.GetLocalSize();
	const FVector2D NodeLocalTL   = ContainerGeo.AbsoluteToLocal(NodeAbsTopLeft);
	const FVector2D NodeLocalBR   = ContainerGeo.AbsoluteToLocal(NodeAbsTopLeft + NodeAbsSize);
	const float     NodeCentreX   = (NodeLocalTL.X + NodeLocalBR.X) * 0.5f;

	const FVector2D PopupSize = NodePopup->GetDesiredSize();

	float PX = NodeCentreX - PopupSize.X * 0.5f;
	float PY = NodeLocalTL.Y - PopupSize.Y;

	if (PY < 0.f) PY = NodeLocalBR.Y;
	PX = FMath::Clamp(PX, 0.f, ContainerSize.X - PopupSize.X);

	if (UCanvasPanelSlot* PopupSlot = Cast<UCanvasPanelSlot>(NodePopup->Slot))
		PopupSlot->SetPosition(FVector2D(PX, PY));
}

void UFP_SkillTreeContainer::HideNodePopup()
{
	if (NodePopup)
		NodePopup->SetVisibility(ESlateVisibility::Collapsed);
}
