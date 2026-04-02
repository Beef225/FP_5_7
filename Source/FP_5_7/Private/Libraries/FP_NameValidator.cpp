// Copyright JG

#include "Libraries/FP_NameValidator.h"
#include "Misc/ConfigCacheIni.h"


// --- Public ---

FString UFP_NameValidator::SanitizeNameInput(const FString& Input)
{
	FString Result;
	Result.Reserve(Input.Len());

	for (const TCHAR C : Input)
	{
		if (IsAllowedCharacter(C))
		{
			Result.AppendChar(C);
		}
	}

	return Result;
}

bool UFP_NameValidator::ValidateCharacterName(const FString& Name, FString& OutError)
{
	if (Name.IsEmpty())
	{
		OutError = TEXT("Name cannot be empty.");
		return false;
	}

	if (Name.Len() < MinLength)
	{
		OutError = FString::Printf(TEXT("Name must be at least %d characters."), MinLength);
		return false;
	}

	if (Name.Len() > MaxLength)
	{
		OutError = FString::Printf(TEXT("Name cannot exceed %d characters."), MaxLength);
		return false;
	}

	// Must contain at least one letter — can't be all underscores
	bool bHasLetter = false;
	for (const TCHAR C : Name) { if (FChar::IsAlpha(C)) { bHasLetter = true; break; } }
	if (!bHasLetter)
	{
		OutError = TEXT("Name must contain at least one letter.");
		return false;
	}

	// Slur check — normalise first to catch spacing/substitution tricks
	const FString Normalized = NormalizeForMatching(Name);
	for (const FString& Term : GetBlockedTerms())
	{
		if (Normalized.Contains(Term, ESearchCase::IgnoreCase))
		{
			OutError = TEXT("That name is not allowed.");
			return false;
		}
	}

	OutError = TEXT("");
	return true;
}

// --- Private ---

bool UFP_NameValidator::IsAllowedCharacter(TCHAR C)
{
	return FChar::IsAlpha(C) || C == TEXT('_');
}

FString UFP_NameValidator::NormalizeForMatching(const FString& Name)
{
	FString Result = Name.ToLower();

	// Common visual substitutions used to evade filters
	const TPair<const TCHAR*, const TCHAR*> Substitutions[] =
	{
		{ TEXT("@"),  TEXT("a") },
		{ TEXT("4"),  TEXT("a") },
		{ TEXT("3"),  TEXT("e") },
		{ TEXT("1"),  TEXT("i") },
		{ TEXT("!"),  TEXT("i") },
		{ TEXT("0"),  TEXT("o") },
		{ TEXT("$"),  TEXT("s") },
		{ TEXT("5"),  TEXT("s") },
		{ TEXT("7"),  TEXT("t") },
		{ TEXT("+"),  TEXT("t") },
		{ TEXT("vv"), TEXT("w") },
	};

	for (const auto& Sub : Substitutions)
	{
		Result = Result.Replace(Sub.Key, Sub.Value);
	}

	// Remove spaces so "w o r d" matches "word"
	Result = Result.Replace(TEXT(" "), TEXT(""));

	// Collapse repeated characters: "niiiig" → "nig"
	FString Collapsed;
	Collapsed.Reserve(Result.Len());
	TCHAR LastChar = TEXT('\0');
	for (const TCHAR C : Result)
	{
		if (C != LastChar)
		{
			Collapsed.AppendChar(C);
			LastChar = C;
		}
	}

	return Collapsed;
}

TArray<FString> UFP_NameValidator::GetBlockedTerms()
{
	TArray<FString> Terms;

	// Read comma-separated blocked terms from DefaultGame.ini:
	//
	// [FP_NameValidation]
	// BlockedTerms=term1,term2,term3
	//
	// Normalise entries the same way as input so the list stays readable.

	FString Raw;
	if (GConfig && GConfig->GetString(
		TEXT("FP_NameValidation"),
		TEXT("BlockedTerms"),
		Raw,
		GGameIni))
	{
		Raw.ParseIntoArray(Terms, TEXT(","), /*bCullEmpty=*/true);

		for (FString& Term : Terms)
		{
			Term = Term.TrimStartAndEnd().ToLower();
		}
	}

	return Terms;
}
