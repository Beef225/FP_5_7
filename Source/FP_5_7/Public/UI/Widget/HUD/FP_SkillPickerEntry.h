// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystem/Data/FP_SkillLibrary.h"
#include "FP_SkillPickerEntry.generated.h"

class UButton;
class UImage;
class UProgressBar;
class URichTextBlock;
class UTextBlock;
class UFP_SkillFrame;

/**
 * One selectable row in UFP_SkillFrame's popup picker.
 * Mirrors UFP_SkillDisplay's icon/name/level/XP-bar layout and adds Button_Select.
 * Clicking Button_Select calls OwningFrame->AssignSkill with this entry's skill tag.
 */
UCLASS()
class FP_5_7_API UFP_SkillPickerEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	void Populate(const FFP_AbilityEntry& Entry, int32 CurrentXP, UFP_SkillFrame* InOwningFrame);

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_SkillIcon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<URichTextBlock> RichText_SkillName;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UTextBlock> Text_SkillLevel;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_SkillXP;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_Select;

private:
	UFUNCTION()
	void OnSelectClicked();

	FGameplayTag SkillTag;
	TWeakObjectPtr<UFP_SkillFrame> OwningFrame;
};
