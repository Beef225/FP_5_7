// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/FP_UserWidget.h"
#include "GameplayTagContainer.h"
#include "Libraries/FP_EnumDefs.h"
#include "UI/Widget/SkillTree/FP_SkillTreeNode.h"
#include "FP_SkillTreeWidget.generated.h"

class UFP_SkillTreeNodeData;
class UCanvasPanel;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillTreePendingCountChanged, int32, NewCount);

/**
 * Runtime skill tree widget.
 *
 * Responsibilities:
 *   - Spawn and position UFP_SkillTreeNode widgets on Canvas_Nodes using
 *     NodePosition values from UFP_SkillTreeNodeData assets.
 *   - Draw connection lines between nodes via NativePaint.
 *   - Compute node states (Ineligible / Available / Pending / Allocated) from
 *     the AllocatedNodeTags set and the pending queue.
 *   - Handle click routing: Available <-> Pending toggle; allocate on Commit.
 *
 * Setup (in BP subclass):
 *   1. Set NodeWidgetClass to the BP subclass of UFP_SkillTreeNode.
 *   2. Fill NodeDataAssets with the data assets created by the Python importer.
 *   3. Bind Canvas_Nodes in the widget hierarchy (must fill the full widget area
 *      so that canvas-space positions match NativePaint local space).
 *   4. Call LoadAllocatedState() on widget open, passing the saved tag container.
 *   5. Bind a Commit button to CommitPendingNodes() and a Cancel button to
 *      CancelPendingNodes().
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class FP_5_7_API UFP_SkillTreeWidget : public UFP_UserWidget
{
	GENERATED_BODY()

public:
	/**
	 * Clears and re-spawns all node widgets from NodeDataAssets.
	 * Called automatically at NativeConstruct. Call again if NodeDataAssets changes.
	 */
	UFUNCTION(BlueprintCallable, Category="Skill Tree")
	void PopulateTree();

	/**
	 * Loads a previously-saved set of allocated tags and refreshes all node states.
	 * Call this immediately after opening the widget, before the player interacts.
	 */
	UFUNCTION(BlueprintCallable, Category="Skill Tree")
	void LoadAllocatedState(const FGameplayTagContainer& InAllocatedNodes);

	/** Allocates every pending node: applies effects, marks Allocated, broadcasts OnNodesCommitted. */
	UFUNCTION(BlueprintCallable, Category="Skill Tree")
	void CommitPendingNodes();

	/** Clears all pending nodes back to Available without allocating them. */
	UFUNCTION(BlueprintCallable, Category="Skill Tree")
	void CancelPendingNodes();

	/** Returns the current allocated tag set — pass to save system on close. */
	UFUNCTION(BlueprintCallable, Category="Skill Tree")
	FGameplayTagContainer GetAllocatedNodeTags() const { return AllocatedNodeTags; }

	UFUNCTION(BlueprintCallable, Category="Skill Tree")
	int32 GetPendingCount() const { return PendingNodeTags.Num(); }

	UFUNCTION(BlueprintCallable, Category="Skill Tree")
	FGameplayTag GetTreeTag() const { return TreeTag; }

	/** Fired whenever the pending count changes — subscribe to drive button states. */
	UPROPERTY(BlueprintAssignable, Category="Skill Tree")
	FOnSkillTreePendingCountChanged OnPendingCountChangedDelegate;

	/** Relays hover enter/leave from any child node widget. */
	FOnSkillTreeNodeHoverChanged OnNodeHoverChangedDelegate;

	/**
	 * Data assets for this tree. Populated automatically by UFP_SkillTreeImporter.
	 * Assign the importer's TargetWidgetClass to this BP and click Import From JSON.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category="Skill Tree|Data")
	TArray<TObjectPtr<UFP_SkillTreeNodeData>> NodeDataAssets;

protected:
	virtual void NativeConstruct() override;

	/**
	 * Draws connection lines between nodes.
	 * Requires Canvas_Nodes to be anchored at (0,0) filling the full widget so
	 * that canvas-space positions match NativePaint local space exactly.
	 */
	virtual int32 NativePaint(
		const FPaintArgs& Args,
		const FGeometry& AllottedGeometry,
		const FSlateRect& MyCullingRect,
		FSlateWindowElementList& OutDrawElements,
		int32 LayerId,
		const FWidgetStyle& InWidgetStyle,
		bool bParentEnabled) const override;

	// ---- BP events ---------------------------------------------------------

	/** Called after pending count changes — show/hide Commit button, point counter, etc. */
	UFUNCTION(BlueprintImplementableEvent, Category="Skill Tree")
	void OnPendingCountChanged(int32 NewCount);

	/** Called after CommitPendingNodes succeeds. */
	UFUNCTION(BlueprintImplementableEvent, Category="Skill Tree")
	void OnNodesCommitted(const TArray<FGameplayTag>& CommittedTags);

	// ---- Config ------------------------------------------------------------

	/**
	 * Gameplay tag identifying this tree (e.g. PassiveTree.Might).
	 * Used to route skill points and save data to the correct tree.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Skill Tree")
	FGameplayTag TreeTag;

	/**
	 * One BP subclass of UFP_SkillTreeNode per node type.
	 * Each class can have a different widget size and visual layout.
	 */
	UPROPERTY(EditDefaultsOnly, Category="Skill Tree|Node Classes")
	TSubclassOf<UFP_SkillTreeNode> NodeWidgetClass_Minor;

	UPROPERTY(EditDefaultsOnly, Category="Skill Tree|Node Classes")
	TSubclassOf<UFP_SkillTreeNode> NodeWidgetClass_Major;

	UPROPERTY(EditDefaultsOnly, Category="Skill Tree|Node Classes")
	TSubclassOf<UFP_SkillTreeNode> NodeWidgetClass_Mastery;

	/** Multiplier applied to NodePosition values from the data assets. */
	UPROPERTY(EditDefaultsOnly, Category="Skill Tree", meta=(ClampMin="0.1"))
	float PositionScale = 1.0f;


	// ---- Line visuals -------------------------------------------------------

	/** Both endpoints Allocated or Pending. */
	UPROPERTY(EditDefaultsOnly, Category="Skill Tree|Lines")
	FLinearColor LineColor_Active = FLinearColor(1.f, 0.8f, 0.2f, 1.f);

	/** One endpoint Allocated / Pending, other not. */
	UPROPERTY(EditDefaultsOnly, Category="Skill Tree|Lines")
	FLinearColor LineColor_Partial = FLinearColor(0.5f, 0.5f, 0.5f, 1.f);

	/** Neither endpoint Allocated nor Pending. */
	UPROPERTY(EditDefaultsOnly, Category="Skill Tree|Lines")
	FLinearColor LineColor_Inactive = FLinearColor(0.15f, 0.15f, 0.15f, 0.7f);

	UPROPERTY(EditDefaultsOnly, Category="Skill Tree|Lines", meta=(ClampMin="0.5"))
	float LineThickness = 2.0f;

	// ---- Widget binding ----------------------------------------------------

	/** Must fill the entire tree widget area (anchors 0,0 → 1,1, offsets 0). */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UCanvasPanel> Canvas_Nodes;

private:
	void SpawnNodeWidget(const UFP_SkillTreeNodeData* Data);
	ESkillTreeNodeState ComputeState(const UFP_SkillTreeNodeData* Data) const;
	void RecomputeAllStates();
	TSubclassOf<UFP_SkillTreeNode> ClassForType(ESkillTreeNodeType Type) const;

	UFUNCTION()
	void HandleNodeClicked(UFP_SkillTreeNode* Node);

	void HandleNodeHoverChanged(UFP_SkillTreeNode* Node, bool bEntered);

	/** Tag → spawned widget. */
	UPROPERTY()
	TMap<FGameplayTag, TObjectPtr<UFP_SkillTreeNode>> SpawnedNodes;

	/** Ordered copy of NodeDataAssets used during paint (avoids TObjectPtr iteration). */
	UPROPERTY()
	TArray<TObjectPtr<const UFP_SkillTreeNodeData>> NodeDataList;

	/** Pre-computed unique connection pairs for NativePaint. Rebuilt in PopulateTree. */
	TArray<TTuple<FGameplayTag, FGameplayTag>> ConnectionPairs;

	FGameplayTagContainer AllocatedNodeTags;
	TArray<FGameplayTag>  PendingNodeTags;
};
