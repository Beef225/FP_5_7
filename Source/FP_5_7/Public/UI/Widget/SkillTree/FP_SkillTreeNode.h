// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/FP_UserWidget.h"
#include "Libraries/FP_EnumDefs.h"
#include "GameplayTagContainer.h"
#include "FP_SkillTreeNode.generated.h"

class UButton;
class UImage;
class USizeBox;
class UFP_SkillTreeNodeData;

DECLARE_MULTICAST_DELEGATE_OneParam(FOnSkillTreeNodeClicked, class UFP_SkillTreeNode* /*Node*/);
DECLARE_MULTICAST_DELEGATE_TwoParams(FOnSkillTreeNodeHoverChanged, class UFP_SkillTreeNode* /*Node*/, bool /*bEntered*/);

/**
 * Base widget for a single skill tree node.
 *
 * State visuals handled in C++:
 *   Ineligible  — button disabled (dark button texture set in BP takes effect automatically).
 *   Available   — button enabled; icon pulses gently between white and PulseColor on a sine wave.
 *   Pending     — button enabled; pulse stopped; icon tint = PendingTint.
 *   Allocated   — button disabled; icon tint = AllocatedTint (slightly bright/warm).
 *
 * Designers subclass in Blueprint and implement OnStateChanged to add any
 * additional visual responses (border swap, size scale, particle, etc.).
 */
UCLASS(Abstract, BlueprintType, Blueprintable)
class FP_5_7_API UFP_SkillTreeNode : public UFP_UserWidget
{
	GENERATED_BODY()

public:
	void Populate(const UFP_SkillTreeNodeData* InNodeData, ESkillTreeNodeState InState);
	void SetState(ESkillTreeNodeState InState);

	FGameplayTag        GetNodeTag()      const;
	ESkillTreeNodeType  GetNodeType()     const;
	ESkillTreeNodeState GetCurrentState() const { return CurrentState; }

	/**
	 * Returns half the SizeBox_Root override dimensions — used by the tree widget
	 * to centre this node on its canvas position.
	 * Falls back to (25, 25) if SizeBox_Root is not bound or has no override set.
	 */
	FVector2D GetDesiredHalfSize() const;

	const UFP_SkillTreeNodeData* GetNodeData() const { return NodeData; }

	FOnSkillTreeNodeClicked      OnNodeClicked;
	FOnSkillTreeNodeHoverChanged OnNodeHoverChanged;

protected:
	virtual void NativeConstruct() override;
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	UFUNCTION(BlueprintImplementableEvent, Category="Skill Tree Node")
	void OnStateChanged(ESkillTreeNodeState NewState, ESkillTreeNodeType Type);

	UPROPERTY(BlueprintReadOnly, Category="Skill Tree Node")
	TObjectPtr<const UFP_SkillTreeNodeData> NodeData;

	UPROPERTY(BlueprintReadOnly, Category="Skill Tree Node")
	ESkillTreeNodeState CurrentState = ESkillTreeNodeState::Ineligible;

	// ---- Tint config (tweak in BP subclass defaults) -----------------------

	/** Icon tint when the node is Ineligible (locked). */
	UPROPERTY(EditDefaultsOnly, Category="Skill Tree Node|Tints")
	FLinearColor IneligibleTint = FLinearColor(0.25f, 0.25f, 0.25f, 1.f);

	/** Icon tint when the node is Allocated. */
	UPROPERTY(EditDefaultsOnly, Category="Skill Tree Node|Tints")
	FLinearColor AllocatedTint = FLinearColor(1.3f, 1.3f, 1.1f, 1.f);

	/** Icon tint when the node is Pending (pre-allocated). */
	UPROPERTY(EditDefaultsOnly, Category="Skill Tree Node|Tints")
	FLinearColor PendingTint = FLinearColor(1.0f, 0.85f, 0.3f, 1.f);

	/** Upper colour of the Available pulse (lower is plain white). */
	UPROPERTY(EditDefaultsOnly, Category="Skill Tree Node|Tints")
	FLinearColor PulseColor = FLinearColor(1.0f, 0.95f, 0.7f, 1.f);

	/** How much of PulseColor is mixed in at peak (0–1). */
	UPROPERTY(EditDefaultsOnly, Category="Skill Tree Node|Tints", meta=(ClampMin="0.0", ClampMax="1.0"))
	float PulseIntensity = 0.35f;

	/** Oscillations per second for the Available pulse. */
	UPROPERTY(EditDefaultsOnly, Category="Skill Tree Node|Tints", meta=(ClampMin="0.1"))
	float PulseSpeed = 1.2f;

	// ---- Widget bindings ---------------------------------------------------

	/** Root size box — its WidthOverride/HeightOverride define the node's canvas footprint. */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<USizeBox> SizeBox_Root;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Node;

	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UImage> Image_NodeIcon;

private:
	UFUNCTION()
	void HandleButtonClicked();

	void ApplyIconTint(const FLinearColor& Tint) const;

	float PulseTime    = 0.f;
	bool  bPulseActive = false;
};
