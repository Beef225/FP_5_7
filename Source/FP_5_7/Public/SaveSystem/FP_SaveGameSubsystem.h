// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "SaveSystem/FP_ProfileSaveData.h"
#include "SaveSystem/FP_LocalUserSaveData.h"
#include "FP_SaveGameSubsystem.generated.h"

class UFP_LocationRegistry;


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFP_OnProfileLoaded);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFP_OnProfileSaved);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FFP_OnCharacterListChanged);

/**
 * Fired when the user identity is resolved and ready.
 *
 * Locally this fires immediately (synchronous).
 * With Steam it may fire one frame later (async identity resolution).
 * The character select screen should wait for this before displaying.
 */
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FFP_OnUserIdentityReady, const FString&, UserID, const FString&, DisplayName);


/**
 * Central save/load manager. Lives on the GameInstance so it survives level transitions.
 * All save operations go through here — nothing touches USaveGame directly.
 *
 * Identity abstraction:
 *   GetUserID() / GetUserDisplayName() return local values now.
 *   When Steam is integrated, override ResolveUserIdentity() to query
 *   the Online Subsystem instead. Everything above this layer stays the same.
 */
UCLASS()
class FP_5_7_API UFP_SaveGameSubsystem : public UGameInstanceSubsystem
{
	GENERATED_BODY()

public:

	// --- Delegates ---

	/**
	 * Fired when user identity is resolved.
	 * Bind to this before trying to show the character select screen.
	 */
	UPROPERTY(BlueprintAssignable)
	FFP_OnUserIdentityReady OnUserIdentityReady;

	/** Fired after the profile is successfully loaded (or a fresh one is created). */
	UPROPERTY(BlueprintAssignable)
	FFP_OnProfileLoaded OnProfileLoaded;

	/** Fired after the profile is written to disk. */
	UPROPERTY(BlueprintAssignable)
	FFP_OnProfileSaved OnProfileSaved;

	/** Fired whenever a character is created or deleted. */
	UPROPERTY(BlueprintAssignable)
	FFP_OnCharacterListChanged OnCharacterListChanged;


	// --- Lifecycle ---

	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Saves the active character's runtime state before shutdown. Fires in PIE and packaged builds. */
	virtual void Deinitialize() override;


	// --- User Identity ---

	/**
	 * Returns the stable unique ID for the current user.
	 * Local: a GUID string generated on first launch.
	 * Steam (future): SteamID as string.
	 */
	UFUNCTION(BlueprintPure, Category="Identity")
	FString GetUserID() const;

	/**
	 * Returns the display name for the current user.
	 * Local: OS username (editable).
	 * Steam (future): Steam display name.
	 */
	UFUNCTION(BlueprintPure, Category="Identity")
	FString GetUserDisplayName() const;

	/**
	 * Update the local display name. No-op when using Steam (Steam owns the name).
	 * Saves identity data to disk.
	 */
	UFUNCTION(BlueprintCallable, Category="Identity")
	void SetUserDisplayName(const FString& NewName);

	UFUNCTION(BlueprintPure, Category="Identity")
	bool IsIdentityReady() const { return bIdentityReady; }


	// --- Profile ---

	/** Loads the profile from disk. Creates a fresh one if none exists. */
	UFUNCTION(BlueprintCallable, Category="Save")
	void LoadProfile();

	/** Writes the current profile to disk. */
	UFUNCTION(BlueprintCallable, Category="Save")
	void SaveProfile();

	UFUNCTION(BlueprintPure, Category="Save")
	bool HasProfile() const { return ProfileData != nullptr; }

	UFUNCTION(BlueprintPure, Category="Save")
	UFP_ProfileSaveData* GetProfile() const { return ProfileData; }


	// --- Characters ---

	/**
	 * Creates a new character record and saves the profile.
	 * Returns false if there are no available slots or the name is empty.
	 */
	UFUNCTION(BlueprintCallable, Category="Save")
	bool CreateCharacter(const FString& Name, const FGameplayTag& ClassTag, FGuid& OutCharacterID);

	/**
	 * Deletes the character with the given ID and saves the profile.
	 * Returns false if the ID was not found.
	 */
	UFUNCTION(BlueprintCallable, Category="Save")
	bool DeleteCharacter(const FGuid& CharacterID);

	/** Returns a copy of all character records, sorted by LastPlayed descending. */
	UFUNCTION(BlueprintPure, Category="Save")
	TArray<FFP_CharacterSaveRecord> GetCharactersSortedByLastPlayed() const;

	UFUNCTION(BlueprintPure, Category="Save")
	bool HasAvailableSlot() const;


	// --- Location Registry ---

	/**
	 * Assign your UFP_LocationRegistry data asset here.
	 * Call this from your GameInstance Blueprint's Init event.
	 * The subsystem survives level transitions, so it only needs to be set once.
	 */
	UFUNCTION(BlueprintCallable, Category="Locations")
	void SetLocationRegistry(UFP_LocationRegistry* InRegistry) { LocationRegistry = InRegistry; }

	UFUNCTION(BlueprintPure, Category="Locations")
	UFP_LocationRegistry* GetLocationRegistry() const { return LocationRegistry; }


	// --- Level Transition ---

	/**
	 * Call this before OpenLevel to tell the game which character to load.
	 * The game level's GameMode reads this on BeginPlay.
	 */
	UFUNCTION(BlueprintCallable, Category="Save")
	void SetPendingCharacter(const FGuid& CharacterID);

	/** Returns the character ID set by SetPendingCharacter. Invalid if none set. */
	UFUNCTION(BlueprintPure, Category="Save")
	FGuid GetPendingCharacterID() const { return PendingCharacterID; }

	/**
	 * Returns a pointer to the pending character's record, or nullptr if none is pending
	 * or the profile isn't loaded.
	 */
	FFP_CharacterSaveRecord* GetPendingCharacterRecord();

	/** Clears the pending character (called by the game level after loading). */
	UFUNCTION(BlueprintCallable, Category="Save")
	void ClearPendingCharacter() { PendingCharacterID.Invalidate(); }

	/**
	 * Resolves the pending character's LastCheckpointTag through the registry
	 * and calls OpenLevel. Call this after SetPendingCharacter.
	 * No-op if the registry is not set or the tag doesn't resolve to a level.
	 */
	UFUNCTION(BlueprintCallable, Category="Save", meta=(WorldContext="WorldContextObject"))
	void OpenLevelForPendingCharacter(const UObject* WorldContextObject);

	/**
	 * Full level-transition flow for in-game travel (doors, portals, etc.):
	 *   1. Saves current runtime state (XP, level, attributes) to disk.
	 *   2. Updates the active character's LastCheckpointTag to LocationTag.
	 *   3. Sets the pending character and saves the profile.
	 *   4. Shows the loading screen and opens the resolved level.
	 */
	UFUNCTION(BlueprintCallable, Category="Save", meta=(WorldContext="WorldContextObject"))
	void TravelToLocation(const UObject* WorldContextObject, const FGameplayTag& LocationTag);


	/**
	 * Reads XP and Level from the active PlayerState and writes them into the
	 * save record for LastPlayedCharacterID, then saves the profile.
	 * No-op if there is no active character or no valid player state.
	 * Call this explicitly before quitting or transitioning levels.
	 */
	UFUNCTION(BlueprintCallable, Category="Save")
	void SaveActiveCharacter();

	/**
	 * Captures and persists the current inventory state (grid + equipped + hover)
	 * for the active character. No-op if no active character or inventory is unavailable.
	 */
	UFUNCTION(BlueprintCallable, Category="Save")
	void SaveInventory();

	/**
	 * Loads the saved inventory for the given character ID and restores it onto
	 * the local player's inventory component.  Call from InventoryComponent::BeginPlay()
	 * after ConstructInventory().
	 */
	void LoadInventory(class UFP_InventoryComponent* InventoryComponent, const FGuid& CharacterID);


	// --- Helpers ---

	static UFP_SaveGameSubsystem* Get(const UObject* WorldContext);

protected:

	/**
	 * Resolves the user identity and fires OnUserIdentityReady when done.
	 *
	 * Local implementation: loads/creates UFP_LocalUserSaveData synchronously.
	 * Steam integration point: override or replace this to query OSS identity
	 * asynchronously before calling FireIdentityReady().
	 */
	virtual void ResolveUserIdentity();

	/** Call this once identity is known to store it and broadcast the delegate. */
	void FireIdentityReady();

private:

	UPROPERTY()
	TObjectPtr<UFP_LocalUserSaveData> LocalUserData;

	UPROPERTY()
	TObjectPtr<UFP_ProfileSaveData> ProfileData;

	UPROPERTY()
	TObjectPtr<UFP_LocationRegistry> LocationRegistry;

	FGuid PendingCharacterID;

	bool bIdentityReady = false;

	/** Profile save slot is per-user: "FP_Profile_<UserID>" */
	FString GetProfileSlotName() const;

	static const FString IdentitySlotName;
	static const int32   SaveUserIndex;
};
