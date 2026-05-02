// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/FP_UserWidget.h"
#include "GameplayTagContainer.h"
#include "FP_SkillTreeContainer.generated.h"

class UFP_SkillTreeWidget;
class UOverlay;
class UButton;

/**
 * Viewport container for a skill tree.
 *
 * Wraps a UFP_SkillTreeWidget inside a scrollable, zoomable canvas.
 *   - Mouse wheel         : zoom in / out, centred on the cursor
 *   - Right-click + drag  : pan; pan speed scales with zoom so the point
 *                           under the cursor stays fixed
 *   - Confirm / Cancel    : forwarded to the inner tree and broadcast upward
 *
 * Setup (in BP subclass):
 *   1. Set TreeWidgetClass to your tree BP.
 *   2. Add Canvas_Viewport (fills the tree area), Button_Confirm, Button_Cancel.
 *   3. Implement OnCommitClicked / OnCancelClicked or bind them in the HUD.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class FP_5_7_API UFP_SkillTreeContainer : public UFP_UserWidget
{
	GENERATED_BODY()

public:
	/** Forward a saved tag set into the inner tree. */
	UFUNCTION(BlueprintCallable, Category="Skill Tree Container")
	void LoadAllocatedState(const FGameplayTagContainer& InAllocated);

	UFUNCTION(BlueprintCallable, Category="Skill Tree Container")
	UFP_SkillTreeWidget* GetTreeWidget() const { return TreeWidget; }

	/** Reset view to default zoom and position. */
	UFUNCTION(BlueprintCallable, Category="Skill Tree Container")
	void ResetView();

protected:
	virtual void NativeConstruct() override;

	virtual FReply NativeOnMouseWheel(
		const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonDown(
		const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseButtonUp(
		const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	virtual FReply NativeOnMouseMove(
		const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	// ---- BP events ---------------------------------------------------------

	UFUNCTION(BlueprintImplementableEvent, Category="Skill Tree Container")
	void OnCommitClicked();

	UFUNCTION(BlueprintImplementableEvent, Category="Skill Tree Container")
	void OnCancelClicked();

	// ---- Config ------------------------------------------------------------

	/** BP subclass of UFP_SkillTreeWidget to spawn inside the viewport. */
	UPROPERTY(EditDefaultsOnly, Category="Skill Tree Container")
	TSubclassOf<UFP_SkillTreeWidget> TreeWidgetClass;

	UPROPERTY(EditDefaultsOnly, Category="Skill Tree Container|Zoom", meta=(ClampMin="0.05"))
	float ZoomMin = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category="Skill Tree Container|Zoom", meta=(ClampMin="0.1"))
	float ZoomMax = 3.0f;

	/** Zoom change per mouse-wheel notch. */
	UPROPERTY(EditDefaultsOnly, Category="Skill Tree Container|Zoom", meta=(ClampMin="0.01"))
	float ZoomStep = 0.12f;

	// ---- Widget bindings ---------------------------------------------------

	/** Fills the tree display area — set anchors to fill in the BP layout. */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UOverlay> Overlay_Viewport;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Confirm;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Cancel;

private:
	void ApplyTransform();

	UFUNCTION() void HandleConfirmClicked();
	UFUNCTION() void HandleCancelClicked();
	UFUNCTION() void HandlePendingCountChanged(int32 NewCount);

	UPROPERTY()
	TObjectPtr<UFP_SkillTreeWidget> TreeWidget;

	float      CurrentZoom = 1.f;
	FVector2D  ViewOffset  = FVector2D::ZeroVector;
	bool       bIsPanning  = false;
};
