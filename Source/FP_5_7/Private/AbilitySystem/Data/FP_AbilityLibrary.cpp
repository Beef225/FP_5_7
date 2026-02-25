// Copyright JG


#include "AbilitySystem/Data/FP_AbilityLibrary.h"

FFP_AbilityEntry UFP_AbilityLibrary::FindAbilityEntryForTag(const FGameplayTag& InSkillTag, bool bLogNotFound) const
{
	for (const FFP_AbilityEntry& Entry : AbilityEntries)
	{
		if (Entry.SkillTag.MatchesTagExact(InSkillTag))
		{
			return Entry;
		}
	}

	if (bLogNotFound)
	{
		UE_LOG(LogTemp, Error, TEXT("Can't find ability entry for SkillTag [%s] on AbilityLibrary [%s]."),
			*InSkillTag.ToString(), *GetNameSafe(this));
	}

	return FFP_AbilityEntry();

}
