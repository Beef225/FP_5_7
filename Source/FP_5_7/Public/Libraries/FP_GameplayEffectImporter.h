#pragma once

#include "Kismet/BlueprintFunctionLibrary.h"
#include "FP_GameplayEffectImporter.generated.h"

UCLASS()
class FP_5_7_API UFPGameplayEffectImporter : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	/**
	 * Editor-only: Reads a CSV/JSON file and creates/updates a Blueprint GameplayEffect class with Override modifiers.
	 *
	 * CSV requires headers:
	 *   Name, InitialValue
	 *
	 * JSON expects an array of objects, each with:
	 *   Name (optional if Attribute is present),
	 *   Attribute (optional if Name is present),
	 *   InitialValue (number or string, optional -> defaults to 0)
	 *
	 * @param CsvFileAbsolutePath  Example: C:/Exports/SecondaryAttributes_Init.json OR .csv
	 * @param AssetPath            Example: /Game/Blueprints/GAS
	 * @param AssetName            Example: GE_InitSecondaryAttributes
	 * @param AttributeSetClass    Example: UFP_AttributeSet::StaticClass()
	 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category="FP|GAS|Import")
	static bool ImportInitGameplayEffectFromCsv(
		const FString& CsvFileAbsolutePath,
		const FString& AssetPath,
		const FString& AssetName,
		TSubclassOf<class UAttributeSet> AttributeSetClass
	);

	/**
	 * Editor-only: Opens a file picker (CSV/JSON) and then imports to a GameplayEffect Blueprint class.
	 *
	 * @param DefaultDirectory Optional start directory for the dialog (can be empty)
	 */
	UFUNCTION(BlueprintCallable, CallInEditor, Category="FP|GAS|Import")
	static bool ImportInitGameplayEffectWithFileDialog(
		const FString& DefaultDirectory,
		const FString& AssetPath,
		const FString& AssetName,
		TSubclassOf<class UAttributeSet> AttributeSetClass
	);
};
