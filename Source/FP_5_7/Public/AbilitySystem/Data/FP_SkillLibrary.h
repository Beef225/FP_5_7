// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "Libraries/FP_EnumDefs.h"
#include "AbilitySystem/Data/FP_SkillLevelUpInfo.h"
#include "FP_SkillLibrary.generated.h"

class UFP_GameplayAbility;
class UTexture2D;

USTRUCT(BlueprintType)
struct FFP_AbilityEntry
{
	GENERATED_BODY();

	/**
	 * Designer-defined default: if true, every new character starts with this skill granted.
	 * This value is a seed — it is read once at character initialisation to populate
	 * AFP_PlayerState::GrantedSkillTags, which is the runtime source of truth.
	 * Do NOT read bGranted from the asset during gameplay to check whether a skill is
	 * currently active; use AFP_PlayerState::IsSkillGranted(SkillTag) instead.
	 * Runtime grant/revoke (items, quests, level-up, etc.) only modifies the PlayerState set.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bGranted = false;

	/** Short category label shown in item tooltips (e.g. "Ranged, Pistol"). */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText DisplayTag;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bShowInSkillMenu = true;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ESkillGrantedBy SkillGrantedBy;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,meta = (Categories = "Skills"))
	FGameplayTag SkillTag = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (Categories = "InputTag"))
	FGameplayTag InputTag = FGameplayTag();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FName SkillName= NAME_None;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UFP_GameplayAbility> SkillAbility;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta=(MultiLine=true))
	FText SkillDescription = FText::GetEmpty();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DamagePerSecond = 0.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BaseSkillSpeed = 1.f;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float BaseRadius = 50.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly,  meta = (Categories = "SkillTagMod"))
	FGameplayTagContainer SkillModifierTags;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UTexture2D> SkillIcon = nullptr;

	/**
	 * XP curve and skill-point award table for this skill.
	 * Leave null for skills that never level up (use MaxLevel = 1 in that case).
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UFP_SkillLevelUpInfo> SkillLevelUpInfo = nullptr;

	/**
	 * Hard cap on this skill's level regardless of XP.
	 * 1  = never levels (e.g. DodgeRoll).
	 * 20 = typical design target at max XP.
	 */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, meta = (ClampMin = 1))
	int32 MaxLevel = 20;

	
	
};


/**
 * 
 */
UCLASS()
class FP_5_7_API UFP_SkillLibrary : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, BlueprintPure)
	FFP_AbilityEntry FindAbilityEntryForTag(const FGameplayTag& InSkillTag, bool bLogNotFound = false) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability Library")
	TArray<FFP_AbilityEntry> AbilityEntries;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Ability Library")
	TObjectPtr<UDataTable> DefaultRichTextStyleSet = nullptr;

};
