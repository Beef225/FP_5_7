// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "AbilitySystem/Data/FP_SkillLibrary.h"
#include "FP_SkillDisplay.generated.h"

class UImage;
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

	void Populate(const FFP_AbilityEntry& Entry, int32 Level);

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_SkillIcon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<URichTextBlock> RichText_SkillName;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_SkillLevel;
};
