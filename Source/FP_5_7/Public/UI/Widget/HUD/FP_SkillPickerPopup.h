// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FP_SkillPickerPopup.generated.h"

class UScrollBox;

/**
 * Viewport-level floating popup that holds the skill picker scroll box.
 * Lives outside SkillFrame's layout so it never displaces the hotbar row.
 */
UCLASS()
class FP_5_7_API UFP_SkillPickerPopup : public UUserWidget
{
	GENERATED_BODY()

public:
	UScrollBox* GetScrollBox() const { return ScrollBox_SkillPicker; }

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UScrollBox> ScrollBox_SkillPicker;
};
