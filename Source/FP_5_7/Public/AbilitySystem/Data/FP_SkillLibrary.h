// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Engine/DataTable.h"
#include "Libraries/FP_EnumDefs.h"
#include "FP_SkillLibrary.generated.h"

class UFP_GameplayAbility;
class UTexture2D;

USTRUCT(BlueprintType)
struct FFP_AbilityEntry
{
	GENERATED_BODY();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	bool bGranted = false;
	
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
