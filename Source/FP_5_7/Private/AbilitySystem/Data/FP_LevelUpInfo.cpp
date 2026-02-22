#include "AbilitySystem/Data/FP_LevelUpInfo.h"

int32 UFP_LevelUpInfo::FindLevelForXP(int32 XP) const
{
	int32 Level = 1;
	bool bSearching = true;

	while (bSearching)
	{
		if (LevelUpInformation.Num() - 1 <= Level) return Level;

		if (XP >= LevelUpInformation[Level].LevelUpRequirement)
		{
			++Level;
		}
		else
		{
			bSearching = false;
		}
	}
	return Level;
}

#if WITH_EDITOR

#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"

void UFP_LevelUpInfo::ImportFromJson()
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!DesktopPlatform)
	{
		UE_LOG(LogTemp, Error, TEXT("ImportFromJson: DesktopPlatform unavailable."));
		return;
	}

	TArray<FString> OutFiles;
	const FString DefaultPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());
	const FString FileTypes = TEXT("JSON files (*.json)|*.json");

	const void* ParentWindowHandle = nullptr;
	const bool bOpened = DesktopPlatform->OpenFileDialog(
		ParentWindowHandle,
		TEXT("Select LevelUp JSON"),
		DefaultPath,
		TEXT(""),
		FileTypes,
		EFileDialogFlags::None,
		OutFiles
	);

	if (!bOpened || OutFiles.Num() == 0) return;

	const FString& JsonPath = OutFiles[0];

	FString JsonText;
	if (!FFileHelper::LoadFileToString(JsonText, *JsonPath))
	{
		UE_LOG(LogTemp, Error, TEXT("ImportFromJson: Failed to read file: %s"), *JsonPath);
		return;
	}

	TSharedPtr<FJsonObject> RootObj;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);

	if (!FJsonSerializer::Deserialize(Reader, RootObj) || !RootObj.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("ImportFromJson: Invalid JSON in %s"), *JsonPath);
		return;
	}

	const TArray<TSharedPtr<FJsonValue>>* LevelArrayPtr = nullptr;
	if (!RootObj->TryGetArrayField(TEXT("LevelUpInformation"), LevelArrayPtr) || !LevelArrayPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("ImportFromJson: Missing array field 'LevelUpInformation'."));
		return;
	}

	TArray<FFP_LevelUpEntry> NewData;
	NewData.Reserve(LevelArrayPtr->Num());

	for (const TSharedPtr<FJsonValue>& Val : *LevelArrayPtr)
	{
		const TSharedPtr<FJsonObject> Entry = Val.IsValid() ? Val->AsObject() : nullptr;
		if (!Entry.IsValid())
		{
			UE_LOG(LogTemp, Warning, TEXT("ImportFromJson: Skipping non-object entry."));
			continue;
		}

		FFP_LevelUpEntry Info;

		double ReqD = 0.0;
		if (Entry->TryGetNumberField(TEXT("LevelUpRequirement"), ReqD))
		{
			Info.LevelUpRequirement = (int32)FMath::Clamp<int64>((int64)llround(ReqD), 0LL, (int64)MAX_int32);
		}

		double AwardD = 1.0;
		if (Entry->TryGetNumberField(TEXT("AttributePointAward"), AwardD))
		{
			Info.AttributePointAward = (int32)FMath::Clamp<int64>((int64)llround(AwardD), 0LL, (int64)MAX_int32);
		}

		NewData.Add(Info);
	}

	if (NewData.Num() < 2)
	{
		UE_LOG(LogTemp, Error, TEXT("ImportFromJson: Not enough entries. Expecting [0] placeholder + levels."));
		return;
	}

	Modify();
	LevelUpInformation = MoveTemp(NewData);

	UE_LOG(LogTemp, Log, TEXT("ImportFromJson: Imported %d entries from %s"), LevelUpInformation.Num(), *JsonPath);
}

#endif
