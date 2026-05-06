// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/FP_UserWidget.h"
#include "GameplayTagContainer.h"
#include "FP_SkillTreeContainer.generated.h"

class UFP_SkillTreeWidget;
class UFP_SkillTreeNode;
class UFP_SkillTreeNodeDescription;
class UWidgetSwitcher;
class UOverlay;
class UButton;
class UTextBlock;
class UCanvasPanel;

/**
 * Unified passive-tree container.
 *
 * Holds all trees in a single UWidgetSwitcher (Switcher_Trees).
 * One set of Confirm / Cancel buttons and point-counter text blocks serves
 * whichever tree is currently active.
 *
 * Setup (BP subclass):
 *   1. Add TreeWidgetClasses entries (one per tree, in tab order).
 *   2. Bind Switcher_Trees, Button_Confirm, Button_Cancel,
 *      Text_PointsSpent, Text_PointsAvailable, (optional) Canvas_Popup.
 *   3. Add tab buttons in BP; on click call SwitchToTree(Index).
 *   4. Implement OnCommitClicked / OnCancelClicked / OnTreeSwitched as needed.
 *   5. Call LoadAllocatedState(TreeIndex, Tags) for each tree on widget open.
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class FP_5_7_API UFP_SkillTreeContainer : public UFP_UserWidget
{
	GENERATED_BODY()

public:
	/** Switch the active tree (0-based, matches TreeWidgetClasses order). */
	UFUNCTION(BlueprintCallable, Category="Skill Tree Container")
	void SwitchToTree(int32 Index);

	/** Load a previously-saved allocated tag set into a specific tree slot. */
	UFUNCTION(BlueprintCallable, Category="Skill Tree Container")
	void LoadAllocatedState(int32 TreeIndex, const FGameplayTagContainer& InAllocated);

	UFUNCTION(BlueprintCallable, Category="Skill Tree Container")
	UFP_SkillTreeWidget* GetActiveTreeWidget() const;

	UFUNCTION(BlueprintCallable, Category="Skill Tree Container")
	int32 GetActiveTreeIndex() const { return ActiveTreeIndex; }

	/** Reset zoom and re-centre the active tree. */
	UFUNCTION(BlueprintCallable, Category="Skill Tree Container")
	void ResetView();

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	virtual FReply NativeOnMouseWheel(
		const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonDown(
		const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseButtonUp(
		const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual FReply NativeOnMouseMove(
		const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;

	// ---- BP events -------------------------------------------------------------

	UFUNCTION(BlueprintImplementableEvent, Category="Skill Tree Container")
	void OnCommitClicked();

	UFUNCTION(BlueprintImplementableEvent, Category="Skill Tree Container")
	void OnCancelClicked();

	/** Fired after SwitchToTree changes the active tree. */
	UFUNCTION(BlueprintImplementableEvent, Category="Skill Tree Container")
	void OnTreeSwitched(int32 NewIndex);

	// ---- Config ----------------------------------------------------------------

	/** One BP subclass of UFP_SkillTreeWidget per tree, in tab display order. */
	UPROPERTY(EditDefaultsOnly, Category="Skill Tree Container|Trees")
	TArray<TSubclassOf<UFP_SkillTreeWidget>> TreeWidgetClasses;

	UPROPERTY(EditDefaultsOnly, Category="Skill Tree Container")
	TSubclassOf<UFP_SkillTreeNodeDescription> NodeDescriptionClass;

	UPROPERTY(EditDefaultsOnly, Category="Skill Tree Container|Zoom", meta=(ClampMin="0.05"))
	float ZoomMin = 0.25f;

	UPROPERTY(EditDefaultsOnly, Category="Skill Tree Container|Zoom", meta=(ClampMin="0.1"))
	float ZoomMax = 3.0f;

	UPROPERTY(EditDefaultsOnly, Category="Skill Tree Container|Zoom", meta=(ClampMin="0.01"))
	float ZoomStep = 0.12f;

	UPROPERTY(EditDefaultsOnly, Category="Skill Tree Container")
	float TreeCanvasSize = 3000.f;

	// ---- Widget bindings -------------------------------------------------------

	/** WidgetSwitcher that holds one overlay per tree. Filled at runtime. */
	UPROPERTY(BlueprintReadOnly, meta=(BindWidget))
	TObjectPtr<UWidgetSwitcher> Switcher_Trees;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Confirm;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Cancel;

	/** Shows allocated count; pending shown as e.g. "12+2". */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_PointsSpent;

	/** Shows remaining available points after pending deduction. */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_PointsAvailable;

	/**
	 * Full-cover canvas for the node description popup.
	 * Must fill the entire container. Set visibility HitTestInvisible in BP.
	 */
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UCanvasPanel> Canvas_Popup;

private:
	// Per-tree pan / zoom state
	struct FTreeViewState
	{
		float     Zoom         = 1.f;
		FVector2D Offset       = FVector2D::ZeroVector;
		bool      bNeedsCenter = true;
	};

	void SpawnTreeWidgets();
	void WireActiveTree();
	void UnwireTree(UFP_SkillTreeWidget* Tree);
	void ApplyTransform();
	void ShowNodePopup(UFP_SkillTreeNode* Node);
	void HideNodePopup();
	void UpdatePointsDisplay(int32 PendingCount);

	UFUNCTION() void HandleConfirmClicked();
	UFUNCTION() void HandleCancelClicked();
	UFUNCTION() void HandlePendingCountChanged(int32 NewCount);
	UFUNCTION() void HandlePassivePointsChanged(FGameplayTag Tag, int32 NewPoints);
	void HandleNodeHoverChanged(UFP_SkillTreeNode* Node, bool bEntered);

	UPROPERTY() TArray<TObjectPtr<UFP_SkillTreeWidget>> TreeWidgets;
	UPROPERTY() TObjectPtr<UFP_SkillTreeNodeDescription> NodePopup;

	TArray<FTreeViewState> ViewStates;
	int32 ActiveTreeIndex       = 0;
	int32 CachedAvailablePoints = 0;
	bool  bIsPanning            = false;
};
