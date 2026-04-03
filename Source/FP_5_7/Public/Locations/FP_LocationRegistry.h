// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "Locations/FP_LocationDataAsset.h"
#include "FP_LocationRegistry.generated.h"


/**
 * Global registry that maps Location tags to their data assets.
 *
 * Create one instance of this asset and assign it to your GameInstance Blueprint.
 * Anything that needs to resolve a gameplay tag to a level (character select,
 * checkpoint loading, fast travel) asks this registry.
 *
 * To add a new location: create a UFP_LocationDataAsset, then add it to the
 * Locations array here. No code changes needed.
 */
UCLASS(BlueprintType)
class FP_5_7_API UFP_LocationRegistry : public UDataAsset
{
	GENERATED_BODY()

public:

	/** All playable locations. Add a new UFP_LocationDataAsset here to register it. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Locations")
	TArray<TObjectPtr<UFP_LocationDataAsset>> Locations;

	/**
	 * The location a completely new character loads into if their save has no
	 * checkpoint tag set. Should point to your starting/tutorial area.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category="Locations", meta=(Categories="Location"))
	FGameplayTag DefaultLocationTag;

	/**
	 * Finds the data asset for a given location tag.
	 * Returns nullptr if the tag is not registered.
	 */
	UFUNCTION(BlueprintPure, Category="Locations")
	UFP_LocationDataAsset* FindLocation(const FGameplayTag& LocationTag) const;

	/**
	 * Resolves the level to load for a character.
	 * If CheckpointTag is invalid or not found, falls back to DefaultLocationTag.
	 * Returns nullptr if neither resolves — caller should handle gracefully.
	 */
	UFUNCTION(BlueprintPure, Category="Locations")
	UFP_LocationDataAsset* ResolveLocationForCharacter(const FGameplayTag& CheckpointTag) const;
};
