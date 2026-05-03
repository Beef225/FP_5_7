// Copyright JG

#include "UI/Widget/SkillTree/FP_SkillTreeContainer.h"
#include "UI/Widget/SkillTree/FP_SkillTreeWidget.h"
#include "UI/Widget/SkillTree/FP_SkillTreeNode.h"
#include "UI/Widget/SkillTree/FP_SkillTreeNodeDescription.h"
#include "Player/FP_PlayerState.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/CanvasPanel.h"
#include "Components/CanvasPanelSlot.h"
#include "Components/Button.h"
#include "Components/TextBlock.h"
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

	if (!TreeWidgetClass || !Overlay_Viewport) return;

	TreeWidget = CreateWidget<UFP_SkillTreeWidget>(GetOwningPlayer(), TreeWidgetClass);
	if (!TreeWidget) return;

	// Fill the overlay so the tree widget occupies the full viewport area.
	// Pan and zoom are applied via render transform — no canvas slot needed.
	UOverlaySlot* OSlot = Overlay_Viewport->AddChildToOverlay(TreeWidget);
	if (OSlot)
	{
		// Left/Top so the overlay uses the tree widget's own desired size
		// (driven by the SizeBox root in the BP) rather than stretching it.
		// Render transform handles all pan/zoom from there.
		OSlot->SetHorizontalAlignment(HAlign_Left);
		OSlot->SetVerticalAlignment(VAlign_Top);
		OSlot->SetPadding(FMargin(0.f));
	}

	// Buttons disabled until the player queues at least one node
	if (Button_Confirm) Button_Confirm->SetIsEnabled(false);
	if (Button_Cancel)  Button_Cancel->SetIsEnabled(false);

	TreeWidget->OnPendingCountChangedDelegate.AddDynamic(
		this, &UFP_SkillTreeContainer::HandlePendingCountChanged);

	TreeWidget->OnNodeHoverChangedDelegate.AddUObject(
		this, &UFP_SkillTreeContainer::HandleNodeHoverChanged);

	// Seed available points from PlayerState and subscribe to future changes
	if (AFP_PlayerState* PS = GetOwningPlayerState<AFP_PlayerState>())
	{
		CachedAvailablePoints = PS->GetPassivePointsForTree(TreeWidget->GetTreeTag());
		PS->OnPassivePointsChangedDelegate.AddUObject(
			this, &UFP_SkillTreeContainer::HandlePassivePointsChanged);
	}
	UpdatePointsDisplay(0);

	// Pivot at top-left so the transform formula is:
	//   ScreenPos = TreeLocalPos * Zoom + ViewOffset
	TreeWidget->SetRenderTransformPivot(FVector2D(0.f, 0.f));
	ApplyTransform();
}

// ---------------------------------------------------------------------------
// View helpers
// ---------------------------------------------------------------------------

void UFP_SkillTreeContainer::ApplyTransform()
{
	if (!TreeWidget) return;
	FWidgetTransform T;
	T.Scale       = FVector2D(CurrentZoom);
	T.Translation = ViewOffset;
	TreeWidget->SetRenderTransform(T);
}

void UFP_SkillTreeContainer::ResetView()
{
	CurrentZoom = 1.f;
	ViewOffset  = FVector2D::ZeroVector;
	ApplyTransform();
}

// ---------------------------------------------------------------------------
// Input — mouse wheel (zoom centred on cursor)
// ---------------------------------------------------------------------------

FReply UFP_SkillTreeContainer::NativeOnMouseWheel(
	const FGeometry& InGeometry, const FPointerEvent& InMouseEvent)
{
	const float NewZoom = FMath::Clamp(
		CurrentZoom + InMouseEvent.GetWheelDelta() * ZoomStep,
		ZoomMin, ZoomMax);

	// Cursor in local (overlay) space
	const FVector2D MouseLocal = InGeometry.AbsoluteToLocal(InMouseEvent.GetScreenSpacePosition());

	// Keep the tree point under the cursor fixed:
	//   ScreenPos = TreePos * Zoom + Offset
	//   NewOffset = MouseLocal - TreePos * NewZoom
	//             = MouseLocal + (Offset - MouseLocal) * (NewZoom / Zoom)
	const float ZoomRatio = NewZoom / CurrentZoom;
	ViewOffset  = MouseLocal + (ViewOffset - MouseLocal) * ZoomRatio;
	CurrentZoom = NewZoom;

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
	if (bIsPanning)
	{
		// Screen-space delta maps 1:1 to offset — panning 1px moves the tree
		// 1px on screen regardless of zoom, which is correct: at higher zoom
		// the same screen drag covers less tree-space distance.
		ViewOffset += InMouseEvent.GetCursorDelta();
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
	if (Button_Confirm) Button_Confirm->SetIsEnabled(bHasPending);
	if (Button_Cancel)  Button_Cancel->SetIsEnabled(bHasPending);
	UpdatePointsDisplay(NewCount);
}

void UFP_SkillTreeContainer::HandlePassivePointsChanged(FGameplayTag Tag, int32 NewPoints)
{
	if (!TreeWidget) return;
	// Only care about changes for our tree's tag (delegate broadcasts attribute tags, not tree tags)
	// We re-read from PlayerState to stay authoritative
	if (AFP_PlayerState* PS = GetOwningPlayerState<AFP_PlayerState>())
		CachedAvailablePoints = PS->GetPassivePointsForTree(TreeWidget->GetTreeTag());
	UpdatePointsDisplay(TreeWidget->GetPendingCount());
}

void UFP_SkillTreeContainer::UpdatePointsDisplay(int32 PendingCount)
{
	if (!TreeWidget) return;

	const int32 Spent = TreeWidget->GetAllocatedNodeTags().Num();

	if (Text_PointsSpent)
	{
		const FText SpentText = PendingCount > 0
			? FText::FromString(FString::Printf(TEXT("%d+%d"), Spent, PendingCount))
			: FText::AsNumber(Spent);
		Text_PointsSpent->SetText(SpentText);
	}

	if (Text_PointsAvailable)
		Text_PointsAvailable->SetText(FText::AsNumber(FMath::Max(0, CachedAvailablePoints - PendingCount)));
}

void UFP_SkillTreeContainer::HandleConfirmClicked()
{
	if (!TreeWidget) return;

	const int32 PointsToSpend = TreeWidget->GetPendingCount();
	TreeWidget->CommitPendingNodes();

	if (PointsToSpend > 0)
		if (AFP_PlayerState* PS = GetOwningPlayerState<AFP_PlayerState>())
			PS->SpendPassivePoints(TreeWidget->GetTreeTag(), PointsToSpend);

	OnCommitClicked();
}

void UFP_SkillTreeContainer::HandleCancelClicked()
{
	if (TreeWidget)
		TreeWidget->CancelPendingNodes();
	OnCancelClicked();
}

void UFP_SkillTreeContainer::LoadAllocatedState(const FGameplayTagContainer& InAllocated)
{
	if (TreeWidget)
		TreeWidget->LoadAllocatedState(InAllocated);
}

// ---------------------------------------------------------------------------
// Node description popup
// ---------------------------------------------------------------------------

void UFP_SkillTreeContainer::HandleNodeHoverChanged(UFP_SkillTreeNode* Node, bool bEntered)
{
	if (bEntered)
		ShowNodePopup(Node);
	else
		HideNodePopup();
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

	// Node bounds in absolute screen space → container-local space
	const FGeometry NodeGeo         = Node->GetCachedGeometry();
	const FVector2D NodeAbsTopLeft  = NodeGeo.GetAbsolutePosition();
	const FVector2D NodeAbsSize     = NodeGeo.GetAbsoluteSize();

	const FGeometry ContainerGeo    = GetCachedGeometry();
	const FVector2D ContainerSize   = ContainerGeo.GetLocalSize();
	const FVector2D NodeLocalTL     = ContainerGeo.AbsoluteToLocal(NodeAbsTopLeft);
	const FVector2D NodeLocalBR     = ContainerGeo.AbsoluteToLocal(NodeAbsTopLeft + NodeAbsSize);
	const float     NodeCentreX     = (NodeLocalTL.X + NodeLocalBR.X) * 0.5f;

	const FVector2D PopupSize       = NodePopup->GetDesiredSize();

	// Default: bottom edge of popup at top edge of node (popup floats above)
	float PX = NodeCentreX - PopupSize.X * 0.5f;
	float PY = NodeLocalTL.Y - PopupSize.Y;

	// Flip below node if popup overflows the top
	if (PY < 0.f)
		PY = NodeLocalBR.Y;

	// Clamp / align horizontally so popup never bleeds off the side
	if (PX < 0.f)
		PX = 0.f;
	else if (PX + PopupSize.X > ContainerSize.X)
		PX = ContainerSize.X - PopupSize.X;

	if (UCanvasPanelSlot* PopupSlot = Cast<UCanvasPanelSlot>(NodePopup->Slot))
		PopupSlot->SetPosition(FVector2D(PX, PY));
}

void UFP_SkillTreeContainer::HideNodePopup()
{
	if (NodePopup)
		NodePopup->SetVisibility(ESlateVisibility::Collapsed);
}
