// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FP_SkillFrameActions.generated.h"

class UButton;
class UFP_SkillFrame;

/**
 * Action bar shown at the top of UFP_SkillFrame's skill picker popup.
 * Button_ClearSlot  — unassigns the skill from the slot and restores the default icon.
 * Button_MoveSlot   — placeholder; move-slot logic to be implemented later.
 */
UCLASS()
class FP_5_7_API UFP_SkillFrameActions : public UUserWidget
{
	GENERATED_BODY()

public:
	void Init(UFP_SkillFrame* InOwningFrame);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_ClearSlot;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_MoveSlot;

private:
	UFUNCTION()
	void OnClearSlotClicked();

	UFUNCTION()
	void OnMoveSlotClicked();

	TWeakObjectPtr<UFP_SkillFrame> OwningFrame;
};
