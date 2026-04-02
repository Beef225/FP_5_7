// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FP_NameValidator.generated.h"


UCLASS()
class FP_5_7_API UFP_NameValidator : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	 * Call on OnTextChanged. Strips anything that isn't a letter or space.
	 * Returns the cleaned string — set your text box's text to this value.
	 */
	UFUNCTION(BlueprintPure, Category="Name Validation")
	static FString SanitizeNameInput(const FString& Input);

	/**
	 * Call on Confirm. Returns true if the name is valid.
	 * OutError is a display-ready message if false (show it under the input box).
	 *
	 * Checks:
	 *  - Not empty
	 *  - Within min/max length
	 *  - No leading or trailing spaces
	 *  - No consecutive spaces
	 *  - Slur list (read from DefaultGame.ini)
	 */
	UFUNCTION(BlueprintPure, Category="Name Validation")
	static bool ValidateCharacterName(const FString& Name, FString& OutError);

private:

	/** Strips numbers, symbols and control characters — keeps letters and spaces only. */
	static bool IsAllowedCharacter(TCHAR C);

	/**
	 * Normalises a name for slur matching:
	 * lowercase, remove spaces, collapse repeated characters,
	 * substitute common visual replacements (@→a, 3→e etc).
	 */
	static FString NormalizeForMatching(const FString& Name);

	/** Loads the blocked term list from [FP_NameValidation] in DefaultGame.ini. */
	static TArray<FString> GetBlockedTerms();

	static const int32 MinLength = 2;
	static const int32 MaxLength = 20;
};
