// Copyright JG

#include "SaveSystem/FP_SaveGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/App.h"
#include "FP_GameplayTags.h"
#include "Locations/FP_LocationRegistry.h"
#include "Locations/FP_LocationDataAsset.h"


const FString UFP_SaveGameSubsystem::IdentitySlotName = TEXT("FP_LocalUser");
const int32   UFP_SaveGameSubsystem::SaveUserIndex    = 0;


void UFP_SaveGameSubsystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);

	// Identity must be resolved before the profile can be loaded,
	// because the profile slot name is derived from the UserID.
	ResolveUserIdentity();
}

// --- User Identity ---

void UFP_SaveGameSubsystem::ResolveUserIdentity()
{
	// Load existing identity, or create a fresh one on first launch.
	if (UGameplayStatics::DoesSaveGameExist(IdentitySlotName, SaveUserIndex))
	{
		LocalUserData = Cast<UFP_LocalUserSaveData>(
			UGameplayStatics::LoadGameFromSlot(IdentitySlotName, SaveUserIndex));
	}

	if (!LocalUserData)
	{
		LocalUserData = Cast<UFP_LocalUserSaveData>(
			UGameplayStatics::CreateSaveGameObject(UFP_LocalUserSaveData::StaticClass()));

		// Generate a stable local ID once, store it forever
		LocalUserData->UserID     = FGuid::NewGuid().ToString(EGuidFormats::DigitsWithHyphens);
		LocalUserData->DisplayName = FPlatformProcess::UserName();

		UGameplayStatics::SaveGameToSlot(LocalUserData, IdentitySlotName, SaveUserIndex);
	}

	FireIdentityReady();
}

void UFP_SaveGameSubsystem::FireIdentityReady()
{
	bIdentityReady = true;

	// Profile slot is now deterministic — load the profile
	LoadProfile();

	OnUserIdentityReady.Broadcast(GetUserID(), GetUserDisplayName());
}

FString UFP_SaveGameSubsystem::GetUserID() const
{
	return LocalUserData ? LocalUserData->UserID : FString();
}

FString UFP_SaveGameSubsystem::GetUserDisplayName() const
{
	return LocalUserData ? LocalUserData->DisplayName : FString();
}

void UFP_SaveGameSubsystem::SetUserDisplayName(const FString& NewName)
{
	if (!LocalUserData || NewName.IsEmpty()) return;

	LocalUserData->DisplayName = NewName;
	UGameplayStatics::SaveGameToSlot(LocalUserData, IdentitySlotName, SaveUserIndex);
}

FString UFP_SaveGameSubsystem::GetProfileSlotName() const
{
	// Each user gets their own profile slot: "FP_Profile_<UserID>"
	// With Steam: UserID becomes the SteamID string — slot names remain unique per account
	return FString::Printf(TEXT("FP_Profile_%s"), *GetUserID());
}

// --- Profile ---

void UFP_SaveGameSubsystem::LoadProfile()
{
	const FString SlotName = GetProfileSlotName();

	if (UGameplayStatics::DoesSaveGameExist(SlotName, SaveUserIndex))
	{
		ProfileData = Cast<UFP_ProfileSaveData>(
			UGameplayStatics::LoadGameFromSlot(SlotName, SaveUserIndex));
	}

	if (!ProfileData)
	{
		ProfileData = Cast<UFP_ProfileSaveData>(
			UGameplayStatics::CreateSaveGameObject(UFP_ProfileSaveData::StaticClass()));
	}

	OnProfileLoaded.Broadcast();
}

void UFP_SaveGameSubsystem::SaveProfile()
{
	if (!ProfileData) return;

	UGameplayStatics::SaveGameToSlot(ProfileData, GetProfileSlotName(), SaveUserIndex);
	OnProfileSaved.Broadcast();
}

// --- Characters ---

bool UFP_SaveGameSubsystem::CreateCharacter(const FString& Name, const FGameplayTag& ClassTag, FGuid& OutCharacterID)
{
	if (!ProfileData) return false;
	if (Name.IsEmpty()) return false;
	if (!ProfileData->HasAvailableSlot()) return false;

	FFP_CharacterSaveRecord& New = ProfileData->AddCharacter(Name, ClassTag);
	New.LastCheckpointTag = FFP_GameplayTags::Get().Location_Startup;
	OutCharacterID = New.CharacterID;

	SaveProfile();
	OnCharacterListChanged.Broadcast();
	return true;
}

bool UFP_SaveGameSubsystem::DeleteCharacter(const FGuid& CharacterID)
{
	if (!ProfileData) return false;
	if (!ProfileData->RemoveCharacter(CharacterID)) return false;

	if (ProfileData->LastPlayedCharacterID == CharacterID)
	{
		ProfileData->LastPlayedCharacterID.Invalidate();
	}

	SaveProfile();
	OnCharacterListChanged.Broadcast();
	return true;
}

TArray<FFP_CharacterSaveRecord> UFP_SaveGameSubsystem::GetCharactersSortedByLastPlayed() const
{
	if (!ProfileData) return {};

	TArray<FFP_CharacterSaveRecord> Sorted = ProfileData->Characters;
	Sorted.Sort([](const FFP_CharacterSaveRecord& A, const FFP_CharacterSaveRecord& B)
	{
		return A.LastPlayed > B.LastPlayed;
	});
	return Sorted;
}

bool UFP_SaveGameSubsystem::HasAvailableSlot() const
{
	return ProfileData && ProfileData->HasAvailableSlot();
}

// --- Level Transition ---

void UFP_SaveGameSubsystem::SetPendingCharacter(const FGuid& CharacterID)
{
	PendingCharacterID = CharacterID;

	if (ProfileData)
	{
		if (FFP_CharacterSaveRecord* Record = ProfileData->FindCharacter(CharacterID))
		{
			Record->LastPlayed = FDateTime::UtcNow();
			ProfileData->LastPlayedCharacterID = CharacterID;
			SaveProfile();
		}
	}
}

FFP_CharacterSaveRecord* UFP_SaveGameSubsystem::GetPendingCharacterRecord()
{
	if (!ProfileData || !PendingCharacterID.IsValid()) return nullptr;
	return ProfileData->FindCharacter(PendingCharacterID);
}

void UFP_SaveGameSubsystem::OpenLevelForPendingCharacter(const UObject* WorldContextObject)
{
	if (!LocationRegistry)
	{
		UE_LOG(LogTemp, Warning, TEXT("FP_SaveGameSubsystem: OpenLevelForPendingCharacter called but LocationRegistry is not set."));
		return;
	}

	const FFP_CharacterSaveRecord* Record = GetPendingCharacterRecord();
	if (!Record)
	{
		UE_LOG(LogTemp, Warning, TEXT("FP_SaveGameSubsystem: OpenLevelForPendingCharacter called but no pending character is set."));
		return;
	}

	const UFP_LocationDataAsset* Location = LocationRegistry->ResolveLocationForCharacter(Record->LastCheckpointTag);
	if (!Location)
	{
		UE_LOG(LogTemp, Warning, TEXT("FP_SaveGameSubsystem: Could not resolve location for tag '%s'. Check the registry DefaultLocationTag."),
			*Record->LastCheckpointTag.ToString());
		return;
	}

	const FName LevelName = Location->GetLevelName();
	if (LevelName.IsNone())
	{
		UE_LOG(LogTemp, Warning, TEXT("FP_SaveGameSubsystem: Location '%s' has no level assigned."), *Location->GetDisplayName().ToString());
		return;
	}

	UGameplayStatics::OpenLevel(WorldContextObject, LevelName);
}

// --- Helpers ---

UFP_SaveGameSubsystem* UFP_SaveGameSubsystem::Get(const UObject* WorldContext)
{
	if (!WorldContext) return nullptr;
	const UWorld* World = WorldContext->GetWorld();
	if (!World) return nullptr;
	const UGameInstance* GI = World->GetGameInstance();
	if (!GI) return nullptr;
	return GI->GetSubsystem<UFP_SaveGameSubsystem>();
}
