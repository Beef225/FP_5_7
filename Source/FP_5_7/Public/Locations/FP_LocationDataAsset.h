// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "FP_LocationDataAsset.generated.h"


UENUM(BlueprintType)
enum class EFP_LevelType : uint8
{
	/** A hand-crafted static level loaded directly. */
	Static		UMETA(DisplayName = "Static"),

	/**
	 * Future: procedurally generated from pre-made room blocks arranged into a maze.
	 * The generator config will be added to this asset when that system is built.
	 */
	Procedural	UMETA(DisplayName = "Procedural")
};


/**
 * Describes a single playable location — the link between a gameplay tag,
 * a UE level asset, and any spawn/generator configuration.
 *
 * One asset per location. Assign to UFP_LocationRegistry so the game can
 * resolve a saved FGameplayTag back to a level at runtime.
 */
UCLASS(BlueprintType)
class FP_5_7_API UFP_LocationDataAsset : public UPrimaryDataAsset
{
	GENERATED_BODY()

public:

	/**
	 * The tag that identifies this location everywhere in the game
	 * (saved on character records, used as checkpoint keys, etc.)
	 * Must start with "Location." — the editor picker is filtered accordingly.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Location", meta=(Categories="Location"))
	FGameplayTag LocationTag;

	/** Display name shown in UI. If left empty, the tag leaf name is used as fallback. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Location")
	FText DisplayName;

	/**
	 * The level to stream/open for this location.
	 * Soft reference — not loaded until the player travels here.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Location")
	TSoftObjectPtr<UWorld> Level;

	/** Controls which loading path is used for this location. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Location")
	EFP_LevelType LevelType = EFP_LevelType::Static;

	/**
	 * Tag identifying where a brand-new character (or one with no saved checkpoint)
	 * spawns when entering this location.
	 *
	 * The checkpoint system will extend this: returning characters will resolve their
	 * saved LastCheckpointTag against spawn points tagged in the level.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Spawn", meta=(Categories="Spawn"))
	FGameplayTag DefaultSpawnTag;

	/** Returns the display name, falling back to the tag's own name if DisplayName is empty. */
	UFUNCTION(BlueprintPure, Category="Location")
	FText GetDisplayName() const;

	/** Returns the level name as an FName, suitable for passing to OpenLevel. */
	UFUNCTION(BlueprintPure, Category="Location")
	FName GetLevelName() const;
};
