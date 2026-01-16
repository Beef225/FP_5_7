// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Libraries/FP_EnumDefs.h"
#include "FP_AttributeInfo.generated.h"

USTRUCT(BlueprintType)
struct FAttributeInfo
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FGameplayTag AttributeTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeName = FText::GetEmpty();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeDescription = FText::GetEmpty();

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	FText AttributeTooltipText = FText::GetEmpty();

	// Runtime only (UI)
	UPROPERTY(BlueprintReadOnly)
	float AttributeValue = 0.f;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	EDisplayType DisplayType = EDisplayType::Numerical;
};

UCLASS(BlueprintType)
class FP_5_7_API UFP_AttributeInfo : public UDataAsset
{
	GENERATED_BODY()

public:
	FAttributeInfo FindAttributeInfoforTag(const FGameplayTag& AttributeTag, bool bLogNotFound = false) const;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TArray<FAttributeInfo> AttributeInformation;

	/** Sanity check button. */
	UFUNCTION(CallInEditor, Category="FP|Debug", meta=(DisplayName="Test Button"))
	void TestButton();

	/** Button: Pick JSON file and REPLACE AttributeInformation. */
	UFUNCTION(CallInEditor, Category="FP|GAS|Import", meta=(DisplayName="Import JSON (Replace)"))
	void ImportFromJson();

	/** Button: Pick JSON file and APPEND to AttributeInformation. */
	UFUNCTION(CallInEditor, Category="FP|GAS|Import", meta=(DisplayName="Import JSON (Append)"))
	void ImportFromJson_Append();

private:
	/** Returns true if it imported at least one row. */
	bool ImportFromJsonFile(const FString& JsonFileAbsolutePath, bool bClearExisting);
	bool PickJsonFile(FString& OutAbsolutePath) const;
};
