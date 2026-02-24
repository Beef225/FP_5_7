// Copyright JG

#include "Input/FP_KeyIconSet.h"
#include "Engine/Texture2D.h"
#include "InputCoreTypes.h" // EKeys

UTexture2D* UFP_KeyIconSet::GetIconForKey(const FKey& Key) const
{
	if (!Key.IsValid())
	{
		return DefaultIcon.Get();
	}

	if (const TObjectPtr<UTexture2D>* Found = KeyToIcon.Find(Key))
	{
		return Found->Get();
	}

	return DefaultIcon.Get();
}

UTexture2D* UFP_KeyIconSet::GetIconForKeyName(FName KeyName) const
{
	FKey Resolved;
	if (!TryResolveKeyFromName(KeyName, Resolved))
	{
		return DefaultIcon;
	}
	return GetIconForKey(Resolved);
}

static FName NormalizeMouseAlias(FName In)
{
	// Handle common aliases you mentioned.
	// Accept: LMB/RMB/MMB and also LeftMouseButton etc.
	const FString S = In.ToString().ToUpper();

	if (S == TEXT("LMB")) return EKeys::LeftMouseButton.GetFName();
	if (S == TEXT("RMB")) return EKeys::RightMouseButton.GetFName();
	if (S == TEXT("MMB")) return EKeys::MiddleMouseButton.GetFName();

	return In;
}

bool UFP_KeyIconSet::TryResolveKeyFromName(FName KeyName, FKey& OutKey)
{
	if (KeyName.IsNone())
		return false;

	KeyName = NormalizeMouseAlias(KeyName);

	// Fast path (exact FName)
	{
		const FKey Key(KeyName);
		if (Key.IsValid())
		{
			OutKey = Key;
			return true;
		}
	}

	// Case-insensitive fallback
	const FString Target = KeyName.ToString().ToUpper();

	TArray<FKey> AllKeys;
	EKeys::GetAllKeys(AllKeys);

	for (const FKey& K : AllKeys)
	{
		if (K.GetFName().ToString().ToUpper() == Target)
		{
			OutKey = K;
			return true;
		}
	}

	return false;
}
