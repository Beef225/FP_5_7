// Copyright JG

#include "SaveSystem/FP_ProfileSaveData.h"


FFP_CharacterSaveRecord* UFP_ProfileSaveData::FindCharacter(const FGuid& ID)
{
	for (FFP_CharacterSaveRecord& Record : Characters)
	{
		if (Record.CharacterID == ID)
		{
			return &Record;
		}
	}
	return nullptr;
}

FFP_CharacterSaveRecord& UFP_ProfileSaveData::AddCharacter(const FString& Name, const FGameplayTag& ClassTag)
{
	FFP_CharacterSaveRecord& New = Characters.AddDefaulted_GetRef();
	New.CharacterID   = FGuid::NewGuid();
	New.CharacterName = Name;
	New.CharacterClassTag = ClassTag;
	New.CharacterLevel = 1;
	New.ExperiencePoints = 0.f;
	New.LastPlayed = FDateTime::UtcNow();
	return New;
}

bool UFP_ProfileSaveData::RemoveCharacter(const FGuid& ID)
{
	const int32 Removed = Characters.RemoveAll([&ID](const FFP_CharacterSaveRecord& R)
	{
		return R.CharacterID == ID;
	});
	return Removed > 0;
}
