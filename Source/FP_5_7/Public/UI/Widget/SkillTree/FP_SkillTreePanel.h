// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/FP_UserWidget.h"
#include "GameplayTagContainer.h"
#include "UI/Widget/SkillTree/FP_SkillTreeWidget.h"
#include "FP_SkillTreePanel.generated.h"

class UNamedSlot;
class UButton;
class UTextBlock;
class UFP_SkillTreeNodeDescription;
class UFP_SkillTreeNode;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FOnSkillTreePanelCommitted,
	FGameplayTag,            SkillTreeTag,
	FGameplayTagContainer,   AllocatedNodes);

UCLASS(Abstract, BlueprintType, Blueprintable)
class FP_5_7_API UFP_SkillTreePanel : public UFP_UserWidget
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category="Skill Tree Panel")
	void OpenForSkill(TSubclassOf<UFP_SkillTreeWidget> TreeWidgetClass,
	                  const FGameplayTagContainer&      AllocatedNodes,
	                  const FText&                      SkillName,
	                  FGameplayTag                      SkillTag);

	UFUNCTION(BlueprintCallable, Category="Skill Tree Panel")
	void Close();

	UFUNCTION(BlueprintPure, Category="Skill Tree Panel")
	UFP_SkillTreeWidget* GetActiveTree() const { return ActiveTree; }

	UPROPERTY(BlueprintAssignable, Category="Skill Tree Panel")
	FOnSkillTreePanelCommitted OnPanelCommitted;

protected:
	virtual void NativeConstruct() override;

	UFUNCTION(BlueprintImplementableEvent, Category="Skill Tree Panel")
	void OnSkillTreeOpened(const FText& InSkillName);

	UFUNCTION(BlueprintImplementableEvent, Category="Skill Tree Panel")
	void OnSkillTreeClosed();

	UPROPERTY(EditDefaultsOnly, Category="Skill Tree Panel")
	TSubclassOf<UFP_SkillTreeNodeDescription> NodeDescriptionClass;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UNamedSlot> Slot_Tree;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Confirm;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Cancel;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_SkillName;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_PointsSpent;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_PointsAvailable;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UTextBlock> Text_PendingCount;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UCanvasPanel> Canvas_Popup;

private:
	void WireActiveTree();
	void UnwireActiveTree();
	void UpdatePointsDisplay(int32 PendingCount);
	void ShowNodePopup(UFP_SkillTreeNode* Node);
	void HideNodePopup();

	UFUNCTION() void HandleConfirmClicked();
	UFUNCTION() void HandleCancelClicked();
	UFUNCTION() void HandlePendingCountChanged(int32 NewCount);
	void HandleNodeHoverChanged(UFP_SkillTreeNode* Node, bool bEntered);

	UPROPERTY() TObjectPtr<UFP_SkillTreeWidget>          ActiveTree;
	UPROPERTY() TObjectPtr<UFP_SkillTreeNodeDescription> NodePopup;

	FGameplayTag CachedSkillTag;
};
