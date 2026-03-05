// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FP_LevelUpInfo.generated.h"

USTRUCT(BlueprintType)
struct FFP_LevelUpEntry
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly)
	int32 LevelUpRequirement = 0;

	UPROPERTY(EditDefaultsOnly)
	int32 AttributePointAward = 1;
};

UCLASS()
class FP_5_7_API UFP_LevelUpInfo : public UDataAsset
{
	GENERATED_BODY()

public:

	// Indexing expectation:
	// LevelUpInformation[1] = Level 1
	// LevelUpInformation[2] = Level 2
	// so [0] is placeholder.
	UPROPERTY(EditDefaultsOnly)
	TArray<FFP_LevelUpEntry> LevelUpInformation;

	UFUNCTION(BlueprintCallable, Category="LevelUp")
	int32 FindLevelForXP(int32 XP) const;

	// Derived from asset size: Num - 1 (placeholder at [0]) - 1 (sentinel at end)
	UFUNCTION(BlueprintCallable, Category="LevelUp")
	int32 GetMaxLevel() const { return FMath::Max(1, LevelUpInformation.Num() - 2); }

#if WITH_EDITOR
	UFUNCTION(CallInEditor, Category="Import")
	void ImportFromJson();
#endif
};
