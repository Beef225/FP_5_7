// Copyright JG

#include "Locations/FP_LocationDataAsset.h"


FText UFP_LocationDataAsset::GetDisplayName() const
{
	if (!DisplayName.IsEmpty()) return DisplayName;

	// Fallback: use the last part of the tag name (e.g. "Location.Lunar_Base" → "Lunar_Base")
	if (LocationTag.IsValid())
	{
		return FText::FromName(LocationTag.GetTagName());
	}

	return FText::FromString(TEXT("Unknown Location"));
}

FName UFP_LocationDataAsset::GetLevelName() const
{
	if (Level.IsNull()) return NAME_None;

	// Extract just the asset name from the full object path
	const FString AssetPath = Level.GetAssetName();
	return FName(*AssetPath);
}
