// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "FP_Leaf.h"
#include "GameplayTagContainer.h"
#include "FP_Leaf_Skills.generated.h"

class AFP_PlayerState;
class UVerticalBox;
class UFP_SkillDisplay;
class UFP_SkillLibrary;

/**
 * Leaf widget that lists the skills granted by an item.
 * Creates one UFP_SkillDisplay child per skill and adds them to Box_Skills.
 * Populated by FFP_SkillFragment::Assimilate when the item description is shown.
 */
UCLASS()
class FP_5_7_API UFP_Leaf_Skills : public UFP_Leaf
{
	GENERATED_BODY()

public:

	void SetSkills(const TArray<FGameplayTag>& SkillTags, const AFP_PlayerState* PS, const UFP_SkillLibrary* Library);

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UVerticalBox> Box_Skills;

	/** Blueprint sub-widget class to instantiate per skill. */
	UPROPERTY(EditAnywhere, Category="Skills")
	TSubclassOf<UFP_SkillDisplay> SkillDisplayClass;
};
