// Copyright JG

#include "SaveSystem/FP_SaveGameSubsystem.h"
#include "Kismet/GameplayStatics.h"
#include "Misc/App.h"
#include "FP_GameplayTags.h"
#include "Locations/FP_LocationRegistry.h"
#include "Locations/FP_LocationDataAsset.h"
#include "Player/FP_PlayerState.h"
#include "GameFramework/PlayerController.h"
#include "Inventory/InventoryManagement/Components/FP_InventoryComponent.h"
#include "Libraries/FP_AbilitySystemLibrary.h"
#include "SaveSystem/FP_InventorySaveData.h"
#include "MoviePlayer.h"
#include "Widgets/SOverlay.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Colors/SColorBlock.h"


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

	// Show loading screen on a background thread while OpenLevel blocks the game thread
	FLoadingScreenAttributes LoadingScreen;
	LoadingScreen.bAutoCompleteWhenLoadingCompletes = true;
	LoadingScreen.MinimumLoadingScreenDisplayTime   = 0.5f;
	LoadingScreen.WidgetLoadingScreen = SNew(SOverlay)
		+ SOverlay::Slot()
		.HAlign(HAlign_Fill)
		.VAlign(VAlign_Fill)
		[
			SNew(SColorBlock)
			.Color(FLinearColor::Black)
		]
		+ SOverlay::Slot()
		.HAlign(HAlign_Center)
		.VAlign(VAlign_Center)
		[
			SNew(STextBlock)
			.Text(FText::FromString(TEXT("Loading...")))
			.ColorAndOpacity(FLinearColor::White)
		];

	GetMoviePlayer()->SetupLoadingScreen(LoadingScreen);
	GetMoviePlayer()->PlayMovie();

	UGameplayStatics::OpenLevel(WorldContextObject, LevelName);
}

void UFP_SaveGameSubsystem::TravelToLocation(const UObject* WorldContextObject, const FGameplayTag& LocationTag)
{
	if (!ProfileData || !ProfileData->LastPlayedCharacterID.IsValid()) return;

	// Flush XP, level, attributes to the record and write to disk
	SaveActiveCharacter();

	// Update checkpoint to the destination so OnPossess in the new level loads here
	FFP_CharacterSaveRecord* Record = ProfileData->FindCharacter(ProfileData->LastPlayedCharacterID);
	if (!Record) return;

	Record->LastCheckpointTag = LocationTag;
	PendingCharacterID = ProfileData->LastPlayedCharacterID;
	SaveProfile();

	OpenLevelForPendingCharacter(WorldContextObject);
}

void UFP_SaveGameSubsystem::Deinitialize()
{
	SaveActiveCharacter();
	Super::Deinitialize();
}

void UFP_SaveGameSubsystem::SaveActiveCharacter()
{
	if (!ProfileData || !ProfileData->LastPlayedCharacterID.IsValid()) return;

	const UWorld* World = GetGameInstance()->GetWorld();
	if (!World) return;

	const APlayerController* PC = World->GetFirstPlayerController();
	if (!PC) return;

	const AFP_PlayerState* PS = PC->GetPlayerState<AFP_PlayerState>();
	if (!PS) return;

	FFP_CharacterSaveRecord* Record = ProfileData->FindCharacter(ProfileData->LastPlayedCharacterID);
	if (!Record) return;

	Record->ExperiencePoints       = PS->GetXP();
	Record->CharacterLevel         = PS->GetPlayerLevel();
	Record->UnspentAttributePoints = PS->GetAttributePoints();
	Record->MightPoints            = PS->GetMightPassivePoints();
	Record->ResonancePoints        = PS->GetResonancePassivePoints();
	Record->AgilityPoints          = PS->GetAgilityPassivePoints();
	Record->FortitudePoints        = PS->GetFortitudePassivePoints();
	Record->SkillXPMap             = PS->GetSkillXPMap();
	Record->SkillLevelMap          = PS->GetSkillLevelMap();
	Record->SkillUnspentPointsMap  = PS->GetSkillUnspentPointsMap();
	Record->SkillInputTagMap       = PS->GetSkillInputTagMap();
	Record->GrantedSkillTagsArray  = PS->GetGrantedSkillTagsArray();

	SaveProfile();
	SaveInventory();
}

void UFP_SaveGameSubsystem::SaveInventory()
{
	if (!ProfileData || !ProfileData->LastPlayedCharacterID.IsValid()) return;

	const UWorld* World = GetGameInstance()->GetWorld();
	if (!World) return;

	APlayerController* PC = World->GetFirstPlayerController();
	if (!PC) return;

	UFP_InventoryComponent* IC = UFP_AbilitySystemLibrary::GetInventoryComponent(PC);
	if (!IsValid(IC)) return;

	UFP_InventorySaveData* SaveData = IC->CaptureInventoryState();
	if (!IsValid(SaveData)) return;

	const FString SlotName = FString::Printf(TEXT("FP_Inventory_%s"),
		*ProfileData->LastPlayedCharacterID.ToString(EGuidFormats::DigitsWithHyphens));

	UGameplayStatics::SaveGameToSlot(SaveData, SlotName, 0);
}

void UFP_SaveGameSubsystem::LoadInventory(UFP_InventoryComponent* InventoryComponent, const FGuid& CharacterID)
{
	if (!IsValid(InventoryComponent) || !CharacterID.IsValid()) return;

	const FString SlotName = FString::Printf(TEXT("FP_Inventory_%s"),
		*CharacterID.ToString(EGuidFormats::DigitsWithHyphens));

	if (!UGameplayStatics::DoesSaveGameExist(SlotName, 0)) return;

	UFP_InventorySaveData* SaveData = Cast<UFP_InventorySaveData>(
		UGameplayStatics::LoadGameFromSlot(SlotName, 0));

	if (!IsValid(SaveData)) return;

	InventoryComponent->RestoreInventoryState(SaveData);
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
