// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UI/Widget/FP_UserWidget.h"
#include "GameplayTagContainer.h"
#include "Libraries/FP_EnumDefs.h"
#include "FP_SkillFrame.generated.h"

class UButton;
class UImage;
class UTexture2D;
class UInputMappingContext;
class UFP_AbilitySystemComponent;
class AFP_PlayerState;
class UFP_InputConfig;
class UFP_KeyIconSet;
class UFP_SkillPickerEntry;
class UFP_SkillFrameActions;
class UFP_SkillPickerPopup;
class UFP_SkillDescription;

/**
 * One slot in the skill hotbar.
 *
 * SlotInputTag is set per-instance in the designer and drives ability activation —
 * it is added to the assigned skill's DynamicSpecSourceTags so AbilityInputTagHeld fires it.
 *
 * Clicking Button_SkillList opens ScrollBox_SkillPicker, which is populated with every
 * currently-granted skill (UFP_SkillPickerEntry, one per skill). Selecting an entry
 * calls AssignSkill, which rebinds the ASC spec and updates the PlayerState map.
 *
 * Image_KeyIcon is resolved automatically from SlotInputTag:
 *   InputTag → UInputAction (via InputConfig) → FKey (via MappingContext) → UTexture2D (via KeyIconSet)
 */
UCLASS()
class FP_5_7_API UFP_SkillFrame : public UFP_UserWidget
{
	GENERATED_BODY()

public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;
	virtual void NativeOnMouseEnter(const FGeometry& InGeometry, const FPointerEvent& InMouseEvent) override;
	virtual void NativeOnMouseLeave(const FPointerEvent& InMouseEvent) override;

	/** Set in the designer for each frame instance — determines which input fires this slot. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Skill Frame")
	FGameplayTag SlotInputTag;

	/** Controls which library entries appear in this frame's picker.
	 *  Standard → entries with bShowInSkillBar.  Injector → entries with bShowInInjectorBar. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Skill Frame")
	ESkillBarType BarType = ESkillBarType::Standard;

	/** Maps InputTag → UInputAction. Set on the frame (or in a parent BP default). */
	UPROPERTY(EditAnywhere, Category="Skill Frame|Key Icon")
	TObjectPtr<UFP_InputConfig> InputConfig;

	/** IMC to query for the key bound to each action (e.g. IMC_FPContext). */
	UPROPERTY(EditAnywhere, Category="Skill Frame|Key Icon")
	TObjectPtr<UInputMappingContext> MappingContext;

	/** Maps FKey → icon texture. */
	UPROPERTY(EditAnywhere, Category="Skill Frame|Key Icon")
	TObjectPtr<UFP_KeyIconSet> KeyIconSet;

	/** Called by UFP_SkillPickerEntry when the player picks a skill from the popup list. */
	void AssignSkill(const FGameplayTag& SkillTag);

	/** Unassigns the current skill: removes the input tag from the ASC spec, clears the
	 *  PlayerState map entry, and restores the default icon. BlueprintCallable for BP access. */
	UFUNCTION(BlueprintCallable, Category="Skill Frame")
	void ClearSlot();

	/** Placeholder — begins a move-to-another-slot flow. Implementation deferred. */
	UFUNCTION(BlueprintCallable, Category="Skill Frame")
	void BeginMoveSlot();

	/** Closes and removes the active picker popup. Called by the popup's backdrop button. */
	void ClosePickerPopup();

protected:
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_SkillIcon;

	/** Displays the key bound to SlotInputTag, resolved at construct time. */
	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UImage> Image_KeyIcon;

	UPROPERTY(meta=(BindWidget))
	TObjectPtr<UButton> Button_SkillList;

	/** Shown when no skill is assigned to this slot. Set in the Blueprint default. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Skill Frame")
	TObjectPtr<UTexture2D> DefaultIcon;

	/** Blueprint sub-widget class to instantiate per granted skill in the picker. */
	UPROPERTY(EditAnywhere, Category="Skill Frame")
	TSubclassOf<UFP_SkillPickerEntry> SkillPickerEntryClass;

	/** Actions bar (clear / move) placed at the top of the picker. */
	UPROPERTY(EditAnywhere, Category="Skill Frame")
	TSubclassOf<UFP_SkillFrameActions> SkillFrameActionsClass;

	/** Floating popup container — must have a bound ScrollBox_SkillPicker. */
	UPROPERTY(EditAnywhere, Category="Skill Frame")
	TSubclassOf<UFP_SkillPickerPopup> SkillPickerPopupClass;

	/** Skill description composite shown on hover. Must derive from UFP_SkillDescription. */
	UPROPERTY(EditAnywhere, Category="Skill Frame")
	TSubclassOf<UFP_SkillDescription> SkillDescriptionClass;

private:
	UFUNCTION()
	void OnSkillListButtonClicked();

	/** Fired by PlayerState when any skill's slot assignment changes. Clears this frame's
	 *  icon if our skill was reassigned to a different slot by another frame. */
	void HandleSkillInputTagAssigned(FGameplayTag SkillTag, FGameplayTag NewInputTag);

	void PopulateSkillPicker();
	void RefreshIcon();
	void RefreshKeyIcon();
	void RestoreFromPlayerState();

	/** Bound to AbilitiesGivenDelegate for late-init when widget constructs before abilities are granted. */
	void OnAbilitiesGiven(UFP_AbilitySystemComponent* ASC);

	AFP_PlayerState* GetFPPlayerState() const;
	UFP_AbilitySystemComponent* GetFPASC() const;

	void OpenDescriptionPopup();
	void CloseDescriptionPopup();

	FGameplayTag AssignedSkillTag;
	FDelegateHandle SkillAssignedHandle;
	TWeakObjectPtr<UFP_SkillPickerPopup> ActivePickerPopup;
	TWeakObjectPtr<UFP_SkillDescription> ActiveDescriptionPopup;
};
