// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystem/Data/FP_SkillLibrary.h"
#include "FP_SkillDisplay.generated.h"

class UImage;
class UProgressBar;
class URichTextBlock;
class UTextBlock;

/**
 * Sub-widget representing a single skill entry on an item tooltip.
 * Contains an icon, skill name (rich text for future formatting), and level label.
 * Created dynamically by UFP_Leaf_Skills — one per skill on the item.
 */
UCLASS()
class FP_5_7_API UFP_SkillDisplay : public UUserWidget
{
	GENERATED_BODY()

public:

	/** CurrentXP is the player's total accumulated XP for this skill (from AFP_PlayerState). */
	void Populate(const FFP_AbilityEntry& Entry, int32 CurrentXP);

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_SkillIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> RichText_SkillName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_SkillLevel;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UProgressBar> ProgressBar_SkillXP;
};
