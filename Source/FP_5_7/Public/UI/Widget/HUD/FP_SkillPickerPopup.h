
// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FP_SkillPickerPopup.generated.h"

class UBorder;
class UButton;
class UScrollBox;
class UFP_SkillFrame;

/**
 * Fullscreen viewport-level popup for the skill picker.
 *
 * BP layout (Canvas Panel root):
 *   Button_Backdrop  — anchors 0,0→1,1 (fills screen), transparent, ZOrder 0
 *   Border_Content   — auto-sized, ZOrder 1, positioned at runtime via PositionContent()
 *     └─ ScrollBox_SkillPicker
 *
 * Clicking outside Border_Content hits Button_Backdrop → closes popup.
 */
UCLASS()
class FP_5_7_API UFP_SkillPickerPopup : public UUserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;

	void Init(UFP_SkillFrame* InOwningFrame);

	UScrollBox* GetScrollBox() const { return ScrollBox_SkillPicker; }

	/** Sets Border_Content's canvas slot so its bottom-centre aligns to AbsoluteAnchorPos (physical pixels). */
	void PositionContent(FVector2D AbsoluteAnchorPos);

protected:
	/** Fullscreen transparent button — catches all off-content clicks. */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Backdrop;

	/** Direct Canvas Panel child that wraps the picker content. Positioned at runtime. */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UBorder> Border_Content;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_SkillPicker;

private:
	UFUNCTION()
	void OnBackdropClicked();

	TWeakObjectPtr<UFP_SkillFrame> OwningFrame;
};
