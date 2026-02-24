// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "Engine/DataAsset.h"
#include "Libraries/FP_EnumDefs.h"
#include "FP_AbilityLibrary.generated.h"



USTRUCT(BlueprintType)\
struct FFP_AbilityEntry
{
	GENERATED_BODY();
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	ESkillGrantedBy SkillGrantedBy;
	
	UPROPERTY(BluePrintReadOnly)
	FGameplayTag SkillTag = FGameplayTag();
	
	UPROPERTY(BlueprintReadOnly)
	FGameplayTag InputTag = FGameplayTag();
	
	UPROPERTY(BlueprintReadOnly)
	FName SkillName= NAME_None;
	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<const UTexture2D> SkillIcon = nullptr;
	
	
};


/**
 * 
 */
UCLASS()
class FP_5_7_API UFP_AbilityLibrary : public UDataAsset
{
	GENERATED_BODY()
};
