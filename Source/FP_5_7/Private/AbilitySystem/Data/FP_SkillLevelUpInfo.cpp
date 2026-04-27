// Copyright JG

#include "AbilitySystem/Data/FP_SkillLevelUpInfo.h"

int32 UFP_SkillLevelUpInfo::FindLevelForXP(int32 XP) const
{
	const int32 MaxLevel = GetMaxLevel();
	int32 Level = 1;

	while (Level < MaxLevel)
	{
		if (XP >= LevelUpInformation[Level + 1].LevelUpRequirement)
			++Level;
		else
			break;
	}
	return Level;
}

int32 UFP_SkillLevelUpInfo::GetXPRequirementForLevel(int32 Level) const
{
	if (!LevelUpInformation.IsValidIndex(Level)) return 0;
	return LevelUpInformation[Level].LevelUpRequirement;
}

int32 UFP_SkillLevelUpInfo::GetTotalSkillPointsAtLevel(int32 Level) const
{
	int32 Total = 0;
	const int32 Clamped = FMath::Min(Level, GetMaxLevel());
	for (int32 i = 1; i <= Clamped; ++i)
	{
		if (LevelUpInformation.IsValidIndex(i))
			Total += LevelUpInformation[i].SkillPointAward;
	}
	return Total;
}

#if WITH_EDITOR

#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"

void UFP_SkillLevelUpInfo::ImportFromJson()
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!DesktopPlatform)
	{
		UE_LOG(LogTemp, Error, TEXT("UFP_SkillLevelUpInfo::ImportFromJson: DesktopPlatform unavailable."));
		return;
	}

	TArray<FString> OutFiles;
	const FString DefaultPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());

	const bool bOpened = DesktopPlatform->OpenFileDialog(
		nullptr,
		TEXT("Select Skill LevelUp JSON"),
		DefaultPath,
		TEXT(""),
		TEXT("JSON files (*.json)|*.json"),
		EFileDialogFlags::None,
		OutFiles
	);

	if (!bOpened || OutFiles.Num() == 0) return;

	FString JsonText;
	if (!FFileHelper::LoadFileToString(JsonText, *OutFiles[0]))
	{
		UE_LOG(LogTemp, Error, TEXT("UFP_SkillLevelUpInfo::ImportFromJson: Failed to read %s"), *OutFiles[0]);
		return;
	}

	TSharedPtr<FJsonObject> RootObj;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);
	if (!FJsonSerializer::Deserialize(Reader, RootObj) || !RootObj.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("UFP_SkillLevelUpInfo::ImportFromJson: Invalid JSON."));
		return;
	}

	const TArray<TSharedPtr<FJsonValue>>* LevelArrayPtr = nullptr;
	if (!RootObj->TryGetArrayField(TEXT("LevelUpInformation"), LevelArrayPtr) || !LevelArrayPtr)
	{
		UE_LOG(LogTemp, Error, TEXT("UFP_SkillLevelUpInfo::ImportFromJson: Missing array 'LevelUpInformation'."));
		return;
	}

	TArray<FFP_SkillLevelUpEntry> NewData;
	NewData.Reserve(LevelArrayPtr->Num());

	for (const TSharedPtr<FJsonValue>& Val : *LevelArrayPtr)
	{
		const TSharedPtr<FJsonObject> Entry = Val.IsValid() ? Val->AsObject() : nullptr;
		if (!Entry.IsValid()) continue;

		FFP_SkillLevelUpEntry Info;

		double ReqD = 0.0;
		if (Entry->TryGetNumberField(TEXT("LevelUpRequirement"), ReqD))
			Info.LevelUpRequirement = (int32)FMath::Clamp<int64>((int64)llround(ReqD), 0LL, (int64)MAX_int32);

		double PointsD = 1.0;
		if (Entry->TryGetNumberField(TEXT("SkillPointAward"), PointsD))
			Info.SkillPointAward = (int32)FMath::Clamp<int64>((int64)llround(PointsD), 0LL, (int64)MAX_int32);

		NewData.Add(Info);
	}

	if (NewData.Num() < 2)
	{
		UE_LOG(LogTemp, Error, TEXT("UFP_SkillLevelUpInfo::ImportFromJson: Need at least [0] placeholder + 1 level."));
		return;
	}

	Modify();
	LevelUpInformation = MoveTemp(NewData);
	UE_LOG(LogTemp, Log, TEXT("UFP_SkillLevelUpInfo::ImportFromJson: Imported %d entries."), LevelUpInformation.Num());
}

#endif
