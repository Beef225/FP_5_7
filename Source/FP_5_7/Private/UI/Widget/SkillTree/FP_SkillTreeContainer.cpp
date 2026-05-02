// Copyright JG

#include "UI/Widget/SkillTree/FP_SkillTreeContainer.h"
#include "UI/Widget/SkillTree/FP_SkillTreeWidget.h"
#include "Components/Overlay.h"
#include "Components/OverlaySlot.h"
#include "Components/Button.h"
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
		OSlot->SetHorizontalAlignment(HAlign_Fill);
		OSlot->SetVerticalAlignment(VAlign_Fill);
		OSlot->SetPadding(FMargin(0.f));
	}

	Overlay_Viewport->SetClipping(EWidgetClipping::ClipToBounds);

	// Buttons disabled until the player queues at least one node
	if (Button_Confirm) Button_Confirm->SetIsEnabled(false);
	if (Button_Cancel)  Button_Cancel->SetIsEnabled(false);

	TreeWidget->OnPendingCountChangedDelegate.AddDynamic(
		this, &UFP_SkillTreeContainer::HandlePendingCountChanged);

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
}

void UFP_SkillTreeContainer::HandleConfirmClicked()
{
	if (TreeWidget)
		TreeWidget->CommitPendingNodes();
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
