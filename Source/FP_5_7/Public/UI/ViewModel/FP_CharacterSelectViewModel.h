// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "MVVMViewModelBase.h"
#include "SaveSystem/FP_ProfileSaveData.h"
#include "FP_CharacterSelectViewModel.generated.h"

class UFP_SaveGameSubsystem;


/**
 * ViewModel for the character select screen.
 *
 * Widgets bind to FieldNotify properties via the View Bindings panel.
 * Commands (Create, Delete, Play) are exposed as BlueprintCallable functions.
 *
 * Initialized by the MainMenuPlayerController after identity is ready.
 */
UCLASS(BlueprintType)
class FP_5_7_API UFP_CharacterSelectViewModel : public UMVVMViewModelBase
{
	GENERATED_BODY()

public:

	/** Call once after the subsystem is ready. Subscribes to save delegates. */
	UFUNCTION(BlueprintCallable, Category="CharacterSelect")
	void Initialize(UFP_SaveGameSubsystem* InSubsystem);

	/** Re-reads character list and user info from the subsystem. */
	UFUNCTION(BlueprintCallable, Category="CharacterSelect")
	void Refresh();


	// --- Commands ---

	/**
	 * Creates a new character with the given name.
	 * Uses a default class tag for now (expandable when class select is added).
	 * Returns false if the name is empty or no slots are available.
	 */
	UFUNCTION(BlueprintCallable, Category="CharacterSelect")
	bool CreateCharacter(const FString& Name);

	/** Deletes the currently selected character. Returns false if nothing is selected. */
	UFUNCTION(BlueprintCallable, Category="CharacterSelect")
	bool DeleteSelectedCharacter();

	/** Deletes a character by explicit ID. Use this from card widgets that own their own delete button. */
	UFUNCTION(BlueprintCallable, Category="CharacterSelect")
	bool DeleteCharacter(const FGuid& CharacterID);

	/** Sets the selected character by ID. Pass an invalid GUID to deselect. */
	UFUNCTION(BlueprintCallable, Category="CharacterSelect")
	void SelectCharacter(const FGuid& CharacterID);

	/** Clears the current selection. */
	UFUNCTION(BlueprintCallable, Category="CharacterSelect")
	void ClearSelection();

	/**
	 * Sets the pending character on the subsystem and triggers level load.
	 * No-op if nothing is selected.
	 */
	UFUNCTION(BlueprintCallable, Category="CharacterSelect")
	void PlaySelectedCharacter();


	// --- Bindable Properties ---

	/** Account-level display name shown at the top of the screen. */
	UFUNCTION(BlueprintPure, FieldNotify, Category="CharacterSelect")
	FString GetUserDisplayName() const { return UserDisplayName; }

	/** All character records, sorted by last played. Bind list views to this. */
	UFUNCTION(BlueprintPure, FieldNotify, Category="CharacterSelect")
	TArray<FFP_CharacterSaveRecord> GetCharacters() const { return Characters; }

	/** ID of the currently selected character. Invalid GUID = nothing selected. */
	UFUNCTION(BlueprintPure, FieldNotify, Category="CharacterSelect")
	FGuid GetSelectedCharacterID() const { return SelectedCharacterID; }

	/** True when a character is selected — enables the Play and Delete buttons. */
	UFUNCTION(BlueprintPure, FieldNotify, Category="CharacterSelect")
	bool GetCanPlay() const { return SelectedCharacterID.IsValid(); }

	/** True when fewer characters exist than the slot limit — enables New Character. */
	UFUNCTION(BlueprintPure, FieldNotify, Category="CharacterSelect")
	bool GetHasAvailableSlot() const { return bHasAvailableSlot; }

	/** Total number of characters — useful for slot counter display. */
	UFUNCTION(BlueprintPure, FieldNotify, Category="CharacterSelect")
	int32 GetCharacterCount() const { return Characters.Num(); }

private:

	void SetUserDisplayName(const FString& InValue);
	void SetCharacters(const TArray<FFP_CharacterSaveRecord>& InValue);
	void SetSelectedCharacterID(const FGuid& InValue);
	void SetHasAvailableSlot(bool InValue);

	UPROPERTY(BlueprintReadOnly, FieldNotify, meta=(AllowPrivateAccess))
	FString UserDisplayName;

	UPROPERTY(BlueprintReadOnly, FieldNotify, meta=(AllowPrivateAccess))
	TArray<FFP_CharacterSaveRecord> Characters;

	UPROPERTY(BlueprintReadOnly, FieldNotify, meta=(AllowPrivateAccess))
	FGuid SelectedCharacterID;

	UPROPERTY(BlueprintReadOnly, FieldNotify, meta=(AllowPrivateAccess))
	bool bHasAvailableSlot = false;

	UPROPERTY()
	TObjectPtr<UFP_SaveGameSubsystem> SaveSubsystem;
};
