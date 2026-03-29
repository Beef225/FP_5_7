// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/SaveGame.h"
#include "FP_LocalUserSaveData.generated.h"


/**
 * Stores the local user's account identity — separate from character data.
 *
 * Currently persisted to disk as a stable local identity.
 * When Steam is integrated, this file is bypassed entirely:
 *   - UserID      → SteamID (as string)
 *   - DisplayName → Steam display name
 * The subsystem exposes GetUserID() / GetUserDisplayName() so nothing
 * above this layer needs to change when Steam comes in.
 */
UCLASS()
class FP_5_7_API UFP_LocalUserSaveData : public USaveGame
{
	GENERATED_BODY()

public:

	/**
	 * Stable unique identifier for this local user.
	 * Generated once on first launch and never changes.
	 * Will be replaced by SteamID (as string) when online.
	 */
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category="Identity")
	FString UserID;

	/**
	 * Human-readable account name shown on the character select screen.
	 * Defaults to the OS username on first launch.
	 * Will be replaced by Steam display name when online.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category="Identity")
	FString DisplayName;
};
