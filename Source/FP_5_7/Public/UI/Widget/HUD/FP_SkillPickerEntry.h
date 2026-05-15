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

/** Fired when the player clicks the skill tree button on an entry.
 *  The parent widget should open UFP_SkillTreePanel::OpenForSkill with this entry. */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnSkillTreeRequested, const FFP_AbilityEntry&, Entry);

/**
 * One selectable row in UFP_SkillFrame's popup picker.
 * Mirrors UFP_SkillDisplay's icon/name/level/XP-bar layout and adds Button_Select.
 * Clicking Button_Select calls OwningFrame->AssignSkill with this entry's skill tag.
 *
 * Button_SkillTree (optional) — visible only when the skill has a SkillTreeWidgetClass set.
 * Clicking it broadcasts OnSkillTreeRequested so the parent can open UFP_SkillTreePanel.
 */
UCLASS()
class FP_5_7_API UFP_SkillPickerEntry : public UUserWidget
{
	GENERATED_BODY()

public:
	void Populate(const FFP_AbilityEntry& Entry, int32 CurrentXP, UFP_SkillFrame* InOwningFrame);

	/** Bind this in the parent picker to open the skill tree panel for the clicked entry. */
	UPROPERTY(BlueprintAssignable, Category="Skill Picker")
	FOnSkillTreeRequested OnSkillTreeRequested;

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

	/**
	 * Optional button that opens this skill's passive tree.
	 * Automatically hidden when the skill has no SkillTreeWidgetClass assigned.
	 * Add a button named Button_SkillTree to the BP widget to enable it.
	 */
	UPROPERTY(meta=(BindWidgetOptional))
	TObjectPtr<UButton> Button_SkillTree;

private:
	UFUNCTION() void OnSelectClicked();
	UFUNCTION() void OnSkillTreeClicked();

	FFP_AbilityEntry CachedEntry;
	TWeakObjectPtr<UFP_SkillFrame> OwningFrame;
};
