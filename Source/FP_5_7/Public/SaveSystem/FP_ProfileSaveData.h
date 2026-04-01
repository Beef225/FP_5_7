// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "GameplayTagContainer.h"
#include "FP_ProfileSaveData.generated.h"


USTRUCT(BlueprintType)
struct FFP_CharacterSaveRecord
{
	GENERATED_BODY()

	/** Unique identifier — stable across renames, deletions, reorders. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGuid CharacterID;

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FString CharacterName;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	int32 CharacterLevel = 1;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	float ExperiencePoints = 0.f;

	/** Identifies the character class (e.g. Player.Class.Warrior). */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FGameplayTag CharacterClassTag;

	/**
	 * Which level/waypoint to load into.
	 * Defaults to invalid — callers should fall back to the default start level.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FGameplayTag LastCheckpointTag;

	/** Used to sort cards by most-recently-played. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly)
	FDateTime LastPlayed;

	bool IsValid() const { return CharacterID.IsValid(); }

	bool operator==(const FFP_CharacterSaveRecord& Other) const { return CharacterID == Other.CharacterID; }
	bool operator!=(const FFP_CharacterSaveRecord& Other) const { return CharacterID != Other.CharacterID; }
};


UCLASS()
class FP_5_7_API UFP_ProfileSaveData : public USaveGame
{
	GENERATED_BODY()

public:

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Profile")
	TArray<FFP_CharacterSaveRecord> Characters;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Profile")
	int32 MaxSlots = 10;

	/** ID of the last character the player loaded into, for pre-selecting on the card list. */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Profile")
	FGuid LastPlayedCharacterID;

	/** Returns the record for a given ID, or nullptr if not found. */
	FFP_CharacterSaveRecord* FindCharacter(const FGuid& ID);

	/** Adds a new record and returns a reference to it. Caller must check slot availability first. */
	FFP_CharacterSaveRecord& AddCharacter(const FString& Name, const FGameplayTag& ClassTag);

	/** Removes the record with the given ID. Returns true if found and removed. */
	bool RemoveCharacter(const FGuid& ID);

	bool HasAvailableSlot() const { return Characters.Num() < MaxSlots; }
};
