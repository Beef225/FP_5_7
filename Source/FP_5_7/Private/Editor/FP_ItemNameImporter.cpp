// Copyright JG

#include "Editor/FP_ItemNameImporter.h"

#if WITH_EDITOR

#include "Inventory/Items/ItemNames/FP_RareNameTable.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "UObject/SavePackage.h"

// ── Helpers (identical pattern to FP_AffixImporter) ──────────────────────────

static TSharedPtr<FJsonObject> LoadJsonFile_Names(const FString& Path)
{
	FString Content;
	if (!FFileHelper::LoadFileToString(Content, *Path))
	{
		UE_LOG(LogTemp, Error, TEXT("FP_ItemNameImporter: cannot read '%s'"), *Path);
		return nullptr;
	}
	TSharedPtr<FJsonObject> Root;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Content);
	if (!FJsonSerializer::Deserialize(Reader, Root))
	{
		UE_LOG(LogTemp, Error, TEXT("FP_ItemNameImporter: JSON parse failed for '%s'"), *Path);
		return nullptr;
	}
	return Root;
}

template<typename T>
static T* GetOrCreateAsset_Names(const FString& PackageName, const FString& AssetName)
{
	UPackage* Package = CreatePackage(*PackageName);
	Package->FullyLoad();
	T* Asset = FindObject<T>(Package, *AssetName);
	if (!Asset)
	{
		Asset = NewObject<T>(Package, *AssetName, RF_Public | RF_Standalone | RF_Transactional);
		FAssetRegistryModule::AssetCreated(Asset);
	}
	return Asset;
}

static void SaveAsset_Names(UObject* Asset)
{
	UPackage* Package = Asset->GetPackage();
	Package->MarkPackageDirty();
	const FString FilePath = FPackageName::LongPackageNameToFilename(
		Package->GetName(), FPackageName::GetAssetPackageExtension());
	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	UPackage::SavePackage(Package, Asset, *FilePath, SaveArgs);
}

// ── Main import ───────────────────────────────────────────────────────────────

void UFP_ItemNameImporter::ImportItemNames()
{
	const FString JsonPath = FPaths::ProjectDir()
		/ TEXT("Tools/MasterDatabase/ItemNames/itemnames.json");

	TSharedPtr<FJsonObject> Root = LoadJsonFile_Names(JsonPath);
	if (!Root) return;

	int32 TablesCreated = 0;

	for (const auto& Pair : Root->Values)
	{
		const TSharedPtr<FJsonObject> Obj = Pair.Value->AsObject();
		if (!Obj) continue;

		// Sanitize sheet name for use as an asset name (replace spaces with underscores)
		FString SafeName = Pair.Key;
		SafeName.ReplaceInline(TEXT(" "), TEXT("_"));

		const FString AssetName = FString::Printf(TEXT("DA_RareName_%s"), *SafeName);
		UFP_RareNameTable* Table = GetOrCreateAsset_Names<UFP_RareNameTable>(
			OutputContentPath / AssetName, AssetName);

		Table->ItemType = FName(*Pair.Key);
		Table->Prefixes.Empty();
		Table->Suffixes.Empty();

		const TArray<TSharedPtr<FJsonValue>>* PrefixArr = nullptr;
		if (Obj->TryGetArrayField(TEXT("Prefixes"), PrefixArr))
		{
			for (const TSharedPtr<FJsonValue>& Val : *PrefixArr)
			{
				FString Word;
				if (Val->TryGetString(Word))
					Table->Prefixes.Add(FName(*Word));
			}
		}

		const TArray<TSharedPtr<FJsonValue>>* SuffixArr = nullptr;
		if (Obj->TryGetArrayField(TEXT("Suffixes"), SuffixArr))
		{
			for (const TSharedPtr<FJsonValue>& Val : *SuffixArr)
			{
				FString Word;
				if (Val->TryGetString(Word))
					Table->Suffixes.Add(FName(*Word));
			}
		}

		SaveAsset_Names(Table);
		++TablesCreated;

		UE_LOG(LogTemp, Log, TEXT("FP_ItemNameImporter: created %s (%d prefixes, %d suffixes)"),
			*AssetName, Table->Prefixes.Num(), Table->Suffixes.Num());
	}

	UE_LOG(LogTemp, Log,
		TEXT("FP_ItemNameImporter: imported %d name tables -> %s"),
		TablesCreated, *OutputContentPath);
}

#endif // WITH_EDITOR
