// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "InputCoreTypes.h" // FKey
#include "FP_KeyIconSet.generated.h"

class UTexture2D;

UCLASS(BlueprintType)
class FP_5_7_API UFP_KeyIconSet : public UDataAsset
{
	GENERATED_BODY()

public:
	// Map any key (keyboard/mouse/gamepad) to an icon texture.
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Key Icons")
	TMap<FKey, TObjectPtr<UTexture2D>> KeyToIcon;

	// Returned when key isn't found (or key invalid).
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category="Key Icons")
	TObjectPtr<UTexture2D> DefaultIcon = nullptr;

	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Key Icons")
	UTexture2D* GetIconForKey(const FKey& Key) const;

	// Optional convenience: accept a string like "A", "1", "LMB", "RMB"
	// and try to resolve to an FKey. Useful if you're still passing names around.
	UFUNCTION(BlueprintCallable, BlueprintPure, Category="Key Icons")
	UTexture2D* GetIconForKeyName(FName KeyName) const;

private:
	static bool TryResolveKeyFromName(FName KeyName, FKey& OutKey);
};
