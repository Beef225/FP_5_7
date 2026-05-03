// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FP_SkillLevelUpInfo.generated.h"

USTRUCT(BlueprintType)
struct FFP_SkillLevelUpEntry
{
	GENERATED_BODY()

	/** Cumulative XP required to reach this skill level. */
	UPROPERTY(EditDefaultsOnly)
	int32 LevelUpRequirement = 0;

	/** Skill points awarded when reaching this level. Spend in the skill's passive tree. */
	UPROPERTY(EditDefaultsOnly)
	int32 SkillPointAward = 1;
};

/**
 * Per-skill XP curve and point award table.
 *
 * Indexing convention (mirrors UFP_LevelUpInfo):
 *   LevelUpInformation[0]  — placeholder (ignored)
 *   LevelUpInformation[1]  — Level 1
 *   LevelUpInformation[N]  — Level N
 *   last entry             — sentinel with the XP total at max level
 *
 * Assign to FFP_AbilityEntry::SkillLevelUpInfo. Leave null for skills that
 * never level (e.g. DodgeRoll with MaxLevel = 1).
 */
UCLASS()
class FP_5_7_API UFP_SkillLevelUpInfo : public UDataAsset
{
	GENERATED_BODY()

public:

	UPROPERTY(EditDefaultsOnly)
	TArray<FFP_SkillLevelUpEntry> LevelUpInformation;

	/** Returns the skill level for the given cumulative XP, capped at GetMaxLevel(). */
	UFUNCTION(BlueprintCallable, Category="SkillLevel")
	int32 FindLevelForXP(int32 XP) const;

	/** Derived from table size: Num - 1 (placeholder) - 1 (sentinel). */
	UFUNCTION(BlueprintCallable, Category="SkillLevel")
	int32 GetMaxLevel() const { return FMath::Max(1, LevelUpInformation.Num() - 2); }

	/** XP required to reach the given level (0 if level is out of range). */
	UFUNCTION(BlueprintCallable, Category="SkillLevel")
	int32 GetXPRequirementForLevel(int32 Level) const;

	/** Total skill points earned by the time the player reaches the given level. */
	UFUNCTION(BlueprintCallable, Category="SkillLevel")
	int32 GetTotalSkillPointsAtLevel(int32 Level) const;

#if WITH_EDITOR
	UFUNCTION(CallInEditor, Category="Import")
	void ImportFromJson();
#endif
};
