// Copyright JG

#include "Locations/FP_LocationRegistry.h"
#include "Locations/FP_LocationDataAsset.h"


UFP_LocationDataAsset* UFP_LocationRegistry::FindLocation(const FGameplayTag& LocationTag) const
{
	if (!LocationTag.IsValid()) return nullptr;

	for (const TObjectPtr<UFP_LocationDataAsset>& Entry : Locations)
	{
		if (Entry && Entry->LocationTag == LocationTag)
		{
			return Entry;
		}
	}

	return nullptr;
}

UFP_LocationDataAsset* UFP_LocationRegistry::ResolveLocationForCharacter(const FGameplayTag& CheckpointTag) const
{
	// Try the character's saved checkpoint first
	if (CheckpointTag.IsValid())
	{
		if (UFP_LocationDataAsset* Found = FindLocation(CheckpointTag))
		{
			return Found;
		}
	}

	// Fall back to the configured default
	if (DefaultLocationTag.IsValid())
	{
		return FindLocation(DefaultLocationTag);
	}

	return nullptr;
}
