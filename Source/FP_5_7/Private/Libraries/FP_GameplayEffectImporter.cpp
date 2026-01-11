#include "Libraries/FP_GameplayEffectImporter.h"

#include "GameplayEffect.h"
#include "AttributeSet.h"

#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/Csv/CsvParser.h"

#if WITH_EDITOR
	#include "AssetRegistry/AssetRegistryModule.h"
	#include "UObject/Package.h"

	#include "Dom/JsonObject.h"
	#include "Serialization/JsonReader.h"
	#include "Serialization/JsonSerializer.h"

	#include "DesktopPlatformModule.h"
	#include "IDesktopPlatform.h"
	#include "Framework/Application/SlateApplication.h"

	#include "Kismet2/KismetEditorUtilities.h"
	#include "Engine/Blueprint.h"
#endif

// -----------------------------
// Helpers (shared)
// -----------------------------

static bool TryGetAttributeProperty(UClass* AttributeSetClass, const FString& Name, FProperty*& OutProp)
{
	OutProp = AttributeSetClass ? AttributeSetClass->FindPropertyByName(*Name) : nullptr;
	return (OutProp != nullptr);
}

static FString StripAttributePrefixIfAny(const FString& In)
{
	FString Out = In.TrimStartAndEnd();
	int32 DotIdx = INDEX_NONE;
	if (Out.FindLastChar(TEXT('.'), DotIdx))
	{
		Out = Out.Mid(DotIdx + 1);
	}
	return Out.TrimStartAndEnd();
}

// -----------------------------
// Editor-only helpers
// -----------------------------
#if WITH_EDITOR

struct FInitRow
{
	FString Name;       // attribute property name, e.g. "Armour"
	float InitialValue = 0.f;
};

static bool LoadRowsFromCsvText(const FString& CsvText, TArray<FInitRow>& OutRows)
{
	FCsvParser Parser(CsvText);
	const TArray<TArray<const TCHAR*>>& Rows = Parser.GetRows();
	if (Rows.Num() < 2)
		return false;

	int32 ColName = INDEX_NONE;
	int32 ColInit = INDEX_NONE;

	const TArray<const TCHAR*>& Header = Rows[0];
	for (int32 c = 0; c < Header.Num(); ++c)
	{
		const FString H = FString(Header[c]).TrimStartAndEnd();
		if (H.Equals(TEXT("Name"), ESearchCase::IgnoreCase)) ColName = c;
		else if (H.Equals(TEXT("InitialValue"), ESearchCase::IgnoreCase)) ColInit = c;
	}

	if (ColName == INDEX_NONE || ColInit == INDEX_NONE)
	{
		UE_LOG(LogTemp, Error, TEXT("[GE Import] CSV missing headers. Required: Name, InitialValue"));
		return false;
	}

	for (int32 r = 1; r < Rows.Num(); ++r)
	{
		const TArray<const TCHAR*>& Row = Rows[r];
		if (Row.Num() <= FMath::Max(ColName, ColInit))
			continue;

		FString Name = FString(Row[ColName]).TrimStartAndEnd();
		FString InitStr = FString(Row[ColInit]).TrimStartAndEnd();

		if (Name.IsEmpty())
			continue;

		FInitRow Out;
		Out.Name = StripAttributePrefixIfAny(Name);

		if (!InitStr.IsEmpty())
		{
			Out.InitialValue = static_cast<float>(FCString::Atod(*InitStr));
		}
		else
		{
			Out.InitialValue = 0.f;
		}

		OutRows.Add(Out);
	}

	return OutRows.Num() > 0;
}

static bool TryReadJsonNumberOrString(const TSharedPtr<FJsonObject>& Obj, const TCHAR* Field, double& OutNumber)
{
	if (!Obj.IsValid())
		return false;

	if (Obj->TryGetNumberField(Field, OutNumber))
		return true;

	FString S;
	if (Obj->TryGetStringField(Field, S))
	{
		S = S.TrimStartAndEnd();
		if (S.IsEmpty())
			return false;

		OutNumber = FCString::Atod(*S);
		return true;
	}

	return false;
}

static bool LoadRowsFromJsonText(const FString& JsonText, TArray<FInitRow>& OutRows)
{
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);

	TArray<TSharedPtr<FJsonValue>> RootArray;
	if (!FJsonSerializer::Deserialize(Reader, RootArray))
	{
		UE_LOG(LogTemp, Error, TEXT("[GE Import] Failed to parse JSON."));
		return false;
	}

	if (RootArray.Num() == 0)
		return false;

	for (const TSharedPtr<FJsonValue>& Val : RootArray)
	{
		const TSharedPtr<FJsonObject>* ObjPtr = nullptr;
		if (!Val.IsValid() || !Val->TryGetObject(ObjPtr) || !ObjPtr || !ObjPtr->IsValid())
			continue;

		const TSharedPtr<FJsonObject>& Obj = *ObjPtr;

		// Name preferred; otherwise fall back to Attribute field like "FP_AttributeSet.Armour"
		FString Name;
		if (!Obj->TryGetStringField(TEXT("Name"), Name))
		{
			FString Attribute;
			if (Obj->TryGetStringField(TEXT("Attribute"), Attribute))
			{
				Name = StripAttributePrefixIfAny(Attribute);
			}
		}

		Name = StripAttributePrefixIfAny(Name);
		if (Name.IsEmpty())
			continue;

		double InitD = 0.0;
		if (!TryReadJsonNumberOrString(Obj, TEXT("InitialValue"), InitD))
		{
			InitD = 0.0; // default
		}

		FInitRow Row;
		Row.Name = Name;
		Row.InitialValue = static_cast<float>(InitD);
		OutRows.Add(Row);
	}

	return OutRows.Num() > 0;
}

static bool LoadRowsFromFileByExtension(const FString& FilePath, TArray<FInitRow>& OutRows)
{
	FString Text;
	if (!FFileHelper::LoadFileToString(Text, *FilePath))
	{
		UE_LOG(LogTemp, Error, TEXT("[GE Import] Failed to load file: %s"), *FilePath);
		return false;
	}

	const FString Ext = FPaths::GetExtension(FilePath, false).ToLower();

	if (Ext == TEXT("json"))
		return LoadRowsFromJsonText(Text, OutRows);

	// default to CSV
	return LoadRowsFromCsvText(Text, OutRows);
}

static bool OpenCsvOrJsonFileDialog(const FString& DefaultDir, FString& OutPickedFile)
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!DesktopPlatform)
		return false;

	void* ParentWindowHandle = nullptr;
	if (FSlateApplication::IsInitialized())
	{
		const void* ParentHandleConst =
			FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);

		ParentWindowHandle = const_cast<void*>(ParentHandleConst);
	}

	const FString Title = TEXT("Select Init Attributes File (.json or .csv)");
	const FString Filter = TEXT("Data Files (*.json;*.csv)|*.json;*.csv|JSON (*.json)|*.json|CSV (*.csv)|*.csv");

	TArray<FString> OutFiles;
	const bool bOk = DesktopPlatform->OpenFileDialog(
		ParentWindowHandle,
		Title,
		DefaultDir,
		TEXT(""),
		Filter,
		0,
		OutFiles
	);

	if (!bOk || OutFiles.Num() == 0)
		return false;

	OutPickedFile = OutFiles[0];
	return true;
}

/**
 * Creates or loads a *Blueprint* GameplayEffect asset and returns its CDO (Class Default Object).
 * This is critical so the resulting asset can be selected in TSubclassOf<UGameplayEffect> pickers.
 */
static UGameplayEffect* CreateOrLoadGameplayEffectBlueprintCDO(const FString& AssetPath, const FString& AssetName)
{
	if (AssetPath.IsEmpty() || AssetName.IsEmpty())
		return nullptr;

	if (!AssetPath.StartsWith(TEXT("/Game")))
	{
		UE_LOG(LogTemp, Error, TEXT("[GE Import] AssetPath must start with /Game. Got: %s"), *AssetPath);
		return nullptr;
	}

	const FString PackageName = AssetPath / AssetName; // "/Game/.../GE_Name"
	UPackage* Package = CreatePackage(*PackageName);
	if (!Package)
		return nullptr;

	// If blueprint already exists in this package, use it
	if (UBlueprint* ExistingBP = FindObject<UBlueprint>(Package, *AssetName))
	{
		if (ExistingBP->GeneratedClass)
		{
			UGameplayEffect* CDO = ExistingBP->GeneratedClass->GetDefaultObject<UGameplayEffect>();
			if (CDO)
			{
				CDO->Modify();
				return CDO;
			}
		}
	}

	// Create new Blueprint derived from UGameplayEffect
	UBlueprint* NewBP = FKismetEditorUtilities::CreateBlueprint(
		UGameplayEffect::StaticClass(),
		Package,
		*AssetName,
		BPTYPE_Normal,
		UBlueprint::StaticClass(),
		UBlueprintGeneratedClass::StaticClass(),
		FName(TEXT("FP_GEImporter"))
	);

	if (!NewBP || !NewBP->GeneratedClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[GE Import] Failed to create GameplayEffect Blueprint: %s/%s"), *AssetPath, *AssetName);
		return nullptr;
	}

	FAssetRegistryModule::AssetCreated(NewBP);
	Package->MarkPackageDirty();

	UGameplayEffect* CDO = NewBP->GeneratedClass->GetDefaultObject<UGameplayEffect>();
	if (CDO)
	{
		CDO->Modify();
	}

	return CDO;
}

#endif // WITH_EDITOR


// -----------------------------
// Public API
// -----------------------------

bool UFPGameplayEffectImporter::ImportInitGameplayEffectFromCsv(
	const FString& CsvFileAbsolutePath,
	const FString& AssetPath,
	const FString& AssetName,
	TSubclassOf<UAttributeSet> AttributeSetClass
)
{
#if !WITH_EDITOR
	return false;
#else
	if (!AttributeSetClass)
	{
		UE_LOG(LogTemp, Error, TEXT("[GE Import] AttributeSetClass is null."));
		return false;
	}

	TArray<FInitRow> Rows;
	if (!LoadRowsFromFileByExtension(CsvFileAbsolutePath, Rows))
		return false;

	// IMPORTANT: create/load *Blueprint* GE class and edit the CDO
	UGameplayEffect* GE = CreateOrLoadGameplayEffectBlueprintCDO(AssetPath, AssetName);
	if (!GE)
		return false;

	GE->Modify();
	GE->DurationPolicy = EGameplayEffectDurationType::Instant;
	GE->Modifiers.Reset();

	UClass* SetClass = AttributeSetClass.Get();

	int32 Added = 0;
	int32 Skipped = 0;

	for (const FInitRow& R : Rows)
	{
		const FString AttrName = StripAttributePrefixIfAny(R.Name);

		FProperty* AttrProp = nullptr;
		if (!TryGetAttributeProperty(SetClass, AttrName, AttrProp))
		{
			UE_LOG(LogTemp, Warning, TEXT("[GE Import] Attribute '%s' not found on AttributeSet '%s'"),
				*AttrName, *GetNameSafe(SetClass));
			++Skipped;
			continue;
		}

		FGameplayModifierInfo Mod;
		Mod.ModifierOp = EGameplayModOp::Override;
		Mod.Attribute = FGameplayAttribute(AttrProp);

		FScalableFloat Mag;
		Mag.Value = R.InitialValue;
		Mod.ModifierMagnitude = FGameplayEffectModifierMagnitude(Mag);

		GE->Modifiers.Add(Mod);
		++Added;
	}

	GE->MarkPackageDirty();
	if (UPackage* Pkg = GE->GetOutermost())
		Pkg->MarkPackageDirty();

	UE_LOG(LogTemp, Display, TEXT("[GE Import] Updated %s/%s from %s. Added=%d Skipped=%d"),
		*AssetPath, *AssetName, *CsvFileAbsolutePath, Added, Skipped);

	return (Added > 0);
#endif
}

bool UFPGameplayEffectImporter::ImportInitGameplayEffectWithFileDialog(
	const FString& DefaultDirectory,
	const FString& AssetPath,
	const FString& AssetName,
	TSubclassOf<UAttributeSet> AttributeSetClass
)
{
#if !WITH_EDITOR
	return false;
#else
	FString PickedFile;
	if (!OpenCsvOrJsonFileDialog(DefaultDirectory, PickedFile))
		return false;

	// Reuse the same importer (supports CSV/JSON by extension)
	return ImportInitGameplayEffectFromCsv(PickedFile, AssetPath, AssetName, AttributeSetClass);
#endif
}
