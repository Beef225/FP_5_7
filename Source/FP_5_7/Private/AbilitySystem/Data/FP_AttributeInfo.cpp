// Copyright JG

#include "AbilitySystem/Data/FP_AttributeInfo.h"

#include "GameplayTagsManager.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"

#if WITH_EDITOR
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "Framework/Application/SlateApplication.h"
#endif

// -------------------------
// Public
// -------------------------

FAttributeInfo UFP_AttributeInfo::FindAttributeInfoforTag(const FGameplayTag& AttributeTag, bool bLogNotFound) const
{
	for (const FAttributeInfo& Info : AttributeInformation)
	{
		if (Info.AttributeTag.MatchesTagExact(AttributeTag))
		{
			return Info;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find Info for AttributeTag [%s] on AttributeInfo [%s]."),
			*AttributeTag.ToString(), *GetNameSafe(this));
	}

	return FAttributeInfo();
}

void UFP_AttributeInfo::TestButton()
{
	UE_LOG(LogTemp, Warning, TEXT("TestButton clicked on [%s]"), *GetNameSafe(this));
}

void UFP_AttributeInfo::ImportFromJson()
{
#if WITH_EDITOR
	FString Path;
	if (!PickJsonFile(Path))
	{
		UE_LOG(LogTemp, Warning, TEXT("[AttributeInfo Import] Cancelled."));
		return;
	}

	const bool bOk = ImportFromJsonFile(Path, /*bClearExisting=*/true);
	UE_LOG(LogTemp, Display, TEXT("[AttributeInfo Import] Replace -> %s"), bOk ? TEXT("SUCCESS") : TEXT("FAILED"));
#else
	UE_LOG(LogTemp, Warning, TEXT("[AttributeInfo Import] Not available in non-editor builds."));
#endif
}

void UFP_AttributeInfo::ImportFromJson_Append()
{
#if WITH_EDITOR
	FString Path;
	if (!PickJsonFile(Path))
	{
		UE_LOG(LogTemp, Warning, TEXT("[AttributeInfo Import] Cancelled."));
		return;
	}

	const bool bOk = ImportFromJsonFile(Path, /*bClearExisting=*/false);
	UE_LOG(LogTemp, Display, TEXT("[AttributeInfo Import] Append -> %s"), bOk ? TEXT("SUCCESS") : TEXT("FAILED"));
#else
	UE_LOG(LogTemp, Warning, TEXT("[AttributeInfo Import] Not available in non-editor builds."));
#endif
}



// -------------------------
// Private helpers
// -------------------------

#if WITH_EDITOR

static bool TryParseDisplayType(const TSharedPtr<FJsonObject>& Obj, EDisplayType& OutType)
{
	if (!Obj.IsValid())
	{
		return false;
	}

	// Accept string: "Numerical" / "Percentage"
	FString TypeStr;
	if (Obj->TryGetStringField(TEXT("DisplayType"), TypeStr))
	{
		TypeStr = TypeStr.TrimStartAndEnd();

		if (TypeStr.Equals(TEXT("Numerical"), ESearchCase::IgnoreCase))
		{
			OutType = EDisplayType::Numerical;
			return true;
		}
		if (TypeStr.Equals(TEXT("Percentage"), ESearchCase::IgnoreCase))
		{
			OutType = EDisplayType::Percentage;
			return true;
		}
	}

	// Accept number: 0 / 1
	double TypeNum = 0.0;
	if (Obj->TryGetNumberField(TEXT("DisplayType"), TypeNum))
	{
		const int32 AsInt = static_cast<int32>(TypeNum);
		if (AsInt == 0) { OutType = EDisplayType::Numerical; return true; }
		if (AsInt == 1) { OutType = EDisplayType::Percentage; return true; }
	}

	return false;
}

static FGameplayTag SafeRequestTag(const FString& TagString, bool& bOutValid)
{
	bOutValid = false;

	const FString Trimmed = TagString.TrimStartAndEnd();
	if (Trimmed.IsEmpty())
	{
		return FGameplayTag();
	}

	// Do NOT auto-add missing tags here.
	const FGameplayTag Tag = UGameplayTagsManager::Get().RequestGameplayTag(FName(*Trimmed), /*ErrorIfNotFound=*/false);
	bOutValid = Tag.IsValid();
	return Tag;
}

#endif // WITH_EDITOR

bool UFP_AttributeInfo::PickJsonFile(FString& OutAbsolutePath) const
{
#if WITH_EDITOR
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!DesktopPlatform)
	{
		UE_LOG(LogTemp, Error, TEXT("[AttributeInfo Import] DesktopPlatform unavailable."));
		return false;
	}

	void* ParentWindowHandle = nullptr;
	if (FSlateApplication::IsInitialized())
	{
		const void* ParentHandleConst = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);
		ParentWindowHandle = const_cast<void*>(ParentHandleConst);
	}

	const FString Title = TEXT("Select AttributeInfo JSON File");
	const FString DefaultPath = FPaths::ProjectDir();
	const FString Filter = TEXT("JSON (*.json)|*.json");

	TArray<FString> OutFiles;
	const bool bOk = DesktopPlatform->OpenFileDialog(
		ParentWindowHandle,
		Title,
		DefaultPath,
		TEXT(""),
		Filter,
		0,
		OutFiles
	);

	if (!bOk || OutFiles.Num() == 0)
	{
		return false;
	}

	OutAbsolutePath = OutFiles[0];
	return true;
#else
	return false;
#endif
}

bool UFP_AttributeInfo::ImportFromJsonFile(const FString& JsonFileAbsolutePath, bool bClearExisting)
{
#if !WITH_EDITOR
	return false;
#else
	if (JsonFileAbsolutePath.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("[AttributeInfo Import] File path is empty."));
		return false;
	}

	FString JsonText;
	if (!FFileHelper::LoadFileToString(JsonText, *JsonFileAbsolutePath))
	{
		UE_LOG(LogTemp, Error, TEXT("[AttributeInfo Import] Failed to load file: %s"), *JsonFileAbsolutePath);
		return false;
	}

	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);

	TArray<TSharedPtr<FJsonValue>> RootArray;
	if (!FJsonSerializer::Deserialize(Reader, RootArray))
	{
		UE_LOG(LogTemp, Error, TEXT("[AttributeInfo Import] Failed to parse JSON: %s"), *JsonFileAbsolutePath);
		return false;
	}

	if (RootArray.Num() == 0)
	{
		UE_LOG(LogTemp, Warning, TEXT("[AttributeInfo Import] JSON contained no rows: %s"), *JsonFileAbsolutePath);
		return false;
	}

	Modify();

	if (bClearExisting)
	{
		AttributeInformation.Reset();
	}

	int32 Added = 0;
	int32 Skipped = 0;
	int32 InvalidTags = 0;

	for (const TSharedPtr<FJsonValue>& Val : RootArray)
	{
		const TSharedPtr<FJsonObject>* ObjPtr = nullptr;
		if (!Val.IsValid() || !Val->TryGetObject(ObjPtr) || ObjPtr == nullptr || !ObjPtr->IsValid())
		{
			++Skipped;
			continue;
		}

		const TSharedPtr<FJsonObject>& Obj = *ObjPtr;

		// Required: AttributeTag
		FString TagStr;
		if (!Obj->TryGetStringField(TEXT("AttributeTag"), TagStr))
		{
			UE_LOG(LogTemp, Warning, TEXT("[AttributeInfo Import] Row missing AttributeTag. Skipping."));
			++Skipped;
			continue;
		}

		bool bTagValid = false;
		const FGameplayTag Tag = SafeRequestTag(TagStr, bTagValid);
		if (!bTagValid)
		{
			UE_LOG(LogTemp, Warning, TEXT("[AttributeInfo Import] GameplayTag not found/invalid: '%s'"), *TagStr);
			++InvalidTags;
			++Skipped;
			continue;
		}

		FAttributeInfo NewInfo;
		NewInfo.AttributeTag = Tag;

		// Optional text fields
		FString S;
		if (Obj->TryGetStringField(TEXT("AttributeName"), S))        { NewInfo.AttributeName = FText::FromString(S); }
		if (Obj->TryGetStringField(TEXT("AttributeDescription"), S)) { NewInfo.AttributeDescription = FText::FromString(S); }
		if (Obj->TryGetStringField(TEXT("AttributeTooltipText"), S)) { NewInfo.AttributeTooltipText = FText::FromString(S); }

		// Optional: DisplayType
		EDisplayType DT = EDisplayType::Numerical;
		if (TryParseDisplayType(Obj, DT))
		{
			NewInfo.DisplayType = DT;
		}
		
		// Optional: DecimalRoundTo
		int32 Decimals = 0;
		if (TryParseDecimalRoundTo(Obj, Decimals))
		{
			NewInfo.DecimalRoundTo = Decimals;
		}

		AttributeInformation.Add(NewInfo);
		++Added;
	}

	MarkPackageDirty();
	if (UPackage* Pkg = GetOutermost())
	{
		Pkg->MarkPackageDirty();
	}

	UE_LOG(LogTemp, Display, TEXT("[AttributeInfo Import] %s: Added=%d Skipped=%d InvalidTags=%d ClearExisting=%s From=%s"),
		*GetNameSafe(this), Added, Skipped, InvalidTags, bClearExisting ? TEXT("true") : TEXT("false"), *JsonFileAbsolutePath);

	return (Added > 0);
#endif
}


bool UFP_AttributeInfo::TryParseDecimalRoundTo(const TSharedPtr<FJsonObject>& Obj, int32& OutValue)
{
	if (!Obj.IsValid())
	{
		return false;
	}

	// Accept number
	double Num = 0.0;
	if (Obj->TryGetNumberField(TEXT("DecimalRoundTo"), Num))
	{
		OutValue = FMath::Max(0, static_cast<int32>(Num)); // clamp non-negative
		return true;
	}

	// Accept string
	FString S;
	if (Obj->TryGetStringField(TEXT("DecimalRoundTo"), S))
	{
		S = S.TrimStartAndEnd();
		if (!S.IsEmpty())
		{
			OutValue = FMath::Max(0, FCString::Atoi(*S));
			return true;
		}
	}

	return false;
}

