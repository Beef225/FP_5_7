// Copyright JG

#include "UI/ViewModel/FP_CharacterSelectViewModel.h"
#include "SaveSystem/FP_SaveGameSubsystem.h"
#include "Kismet/GameplayStatics.h"


void UFP_CharacterSelectViewModel::Initialize(UFP_SaveGameSubsystem* InSubsystem)
{
	SaveSubsystem = InSubsystem;
	if (!SaveSubsystem) return;

	// Refresh whenever character list changes (create/delete)
	SaveSubsystem->OnCharacterListChanged.AddDynamic(this, &UFP_CharacterSelectViewModel::Refresh);

	Refresh();
}

void UFP_CharacterSelectViewModel::Refresh()
{
	if (!SaveSubsystem) return;

	SetUserDisplayName(SaveSubsystem->GetUserDisplayName());
	SetCharacters(SaveSubsystem->GetCharactersSortedByLastPlayed());
	SetHasAvailableSlot(SaveSubsystem->HasAvailableSlot());

	// If the selected character was deleted, clear selection
	if (SelectedCharacterID.IsValid())
	{
		const bool bStillExists = Characters.ContainsByPredicate([this](const FFP_CharacterSaveRecord& R)
		{
			return R.CharacterID == SelectedCharacterID;
		});

		if (!bStillExists)
		{
			SetSelectedCharacterID(FGuid());
		}
	}
}

// --- Commands ---

bool UFP_CharacterSelectViewModel::CreateCharacter(const FString& Name)
{
	if (!SaveSubsystem) return false;

	FGuid NewID;
	// Default class tag — empty for now, will be set when class selection is added
	const bool bSuccess = SaveSubsystem->CreateCharacter(Name, FGameplayTag(), NewID);

	if (bSuccess)
	{
		// Auto-select the newly created character
		SetSelectedCharacterID(NewID);
	}

	return bSuccess;
}

bool UFP_CharacterSelectViewModel::DeleteCharacter(const FGuid& CharacterID)
{
	if (!SaveSubsystem || !CharacterID.IsValid()) return false;

	if (SelectedCharacterID == CharacterID)
	{
		SetSelectedCharacterID(FGuid());
	}

	return SaveSubsystem->DeleteCharacter(CharacterID);
}

bool UFP_CharacterSelectViewModel::DeleteSelectedCharacter()
{
	if (!SaveSubsystem || !SelectedCharacterID.IsValid()) return false;

	const FGuid IDToDelete = SelectedCharacterID;
	SetSelectedCharacterID(FGuid()); // Clear selection before delete
	return SaveSubsystem->DeleteCharacter(IDToDelete);
}

void UFP_CharacterSelectViewModel::SelectCharacter(const FGuid& CharacterID)
{
	SetSelectedCharacterID(CharacterID);
}

void UFP_CharacterSelectViewModel::ClearSelection()
{
	SetSelectedCharacterID(FGuid());
}

void UFP_CharacterSelectViewModel::PlaySelectedCharacter()
{
	if (!SaveSubsystem || !SelectedCharacterID.IsValid()) return;

	SaveSubsystem->SetPendingCharacter(SelectedCharacterID);

	// TODO: replace with your actual game level name
	UGameplayStatics::OpenLevel(this, FName("DefaultMap"));
}

// --- Private Setters (FieldNotify) ---

void UFP_CharacterSelectViewModel::SetUserDisplayName(const FString& InValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(UserDisplayName, InValue);
}

void UFP_CharacterSelectViewModel::SetCharacters(const TArray<FFP_CharacterSaveRecord>& InValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(Characters, InValue);
}

void UFP_CharacterSelectViewModel::SetSelectedCharacterID(const FGuid& InValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(SelectedCharacterID, InValue);
	// bCanPlay and CharacterCount are computed from other fields — notify them too
	UE_MVVM_BROADCAST_FIELD_VALUE_CHANGED(GetCanPlay);
}

void UFP_CharacterSelectViewModel::SetHasAvailableSlot(bool InValue)
{
	UE_MVVM_SET_PROPERTY_VALUE(bHasAvailableSlot, InValue);
}
