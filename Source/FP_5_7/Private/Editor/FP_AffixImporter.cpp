// Copyright JG

#include "Editor/FP_AffixImporter.h"

#if WITH_EDITOR

#include "Inventory/Items/Affixes/FP_AffixDefinition.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Dom/JsonObject.h"
#include "Dom/JsonValue.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "UObject/SavePackage.h"

// ── Helpers ───────────────────────────────────────────────────────────────────

static TSharedPtr<FJsonObject> LoadJsonFile(const FString& Path)
{
	FString Content;
	if (!FFileHelper::LoadFileToString(Content, *Path))
	{
		UE_LOG(LogTemp, Error, TEXT("FP_AffixImporter: cannot read '%s'"), *Path);
		return nullptr;
	}
	TSharedPtr<FJsonObject> Root;
	TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(Content);
	if (!FJsonSerializer::Deserialize(Reader, Root))
	{
		UE_LOG(LogTemp, Error, TEXT("FP_AffixImporter: JSON parse failed for '%s'"), *Path);
		return nullptr;
	}
	return Root;
}

template<typename T>
static T* GetOrCreateAsset(const FString& PackageName, const FString& AssetName)
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

static void SaveAsset(UObject* Asset)
{
	UPackage* Package = Asset->GetPackage();
	Package->MarkPackageDirty();
	const FString FilePath = FPackageName::LongPackageNameToFilename(
		Package->GetName(), FPackageName::GetAssetPackageExtension());
	FSavePackageArgs SaveArgs;
	SaveArgs.TopLevelFlags = RF_Public | RF_Standalone;
	UPackage::SavePackage(Package, Asset, *FilePath, SaveArgs);
}

static EFP_AffixType    ParseAffixType (const FString& S) { return S == TEXT("Suffix") ? EFP_AffixType::Suffix   : EFP_AffixType::Prefix;   }
static EFP_AffixDisplayAs ParseDisplayAs(const FString& S) { return S == TEXT("Percent") ? EFP_AffixDisplayAs::Percent : EFP_AffixDisplayAs::Flat; }

static FGameplayTag ParseTag(const FString& S)
{
	if (S.IsEmpty()) return FGameplayTag::EmptyTag;
	return FGameplayTag::RequestGameplayTag(FName(*S), false);
}

static FFP_AffixTierData ParseTier(const TSharedPtr<FJsonObject>& Obj)
{
	FFP_AffixTierData T;
	double N = 0.0;
	FString S;

	Obj->TryGetNumberField(TEXT("Tier"),               N); T.Tier               = (int32)N; N = 0.0;
	Obj->TryGetStringField(TEXT("ItemName"),            S); T.ItemName           = FName(*S); S.Empty();
	Obj->TryGetNumberField(TEXT("RequiredItemLevel"),   N); T.RequiredItemLevel  = (int32)N; N = 0.0;
	Obj->TryGetNumberField(TEXT("Weight"),              N); T.Weight             = (int32)N; N = 0.0;
	Obj->TryGetNumberField(TEXT("Stat1_Min"),           N); T.Stat1_Min          = (float)N;  N = 0.0;
	Obj->TryGetNumberField(TEXT("Stat1_Max"),           N); T.Stat1_Max          = (float)N;  N = 0.0;
	Obj->TryGetNumberField(TEXT("Stat1_Step"),          N); T.Stat1_Step         = (float)N;  N = 0.0;
	Obj->TryGetNumberField(TEXT("Stat1_DecimalCount"),  N); T.Stat1_DecimalCount = (int32)N; N = 0.0;
	if (Obj->TryGetStringField(TEXT("Stat1_DisplayAs"), S)) T.Stat1_DisplayAs    = ParseDisplayAs(S); S.Empty();

	Obj->TryGetNumberField(TEXT("Stat2_Min"),           N); T.Stat2_Min          = (float)N;  N = 0.0;
	Obj->TryGetNumberField(TEXT("Stat2_Max"),           N); T.Stat2_Max          = (float)N;  N = 0.0;
	Obj->TryGetNumberField(TEXT("Stat2_Step"),          N); T.Stat2_Step         = (float)N;  N = 0.0;
	Obj->TryGetNumberField(TEXT("Stat2_DecimalCount"),  N); T.Stat2_DecimalCount = (int32)N;
	if (Obj->TryGetStringField(TEXT("Stat2_DisplayAs"), S)) T.Stat2_DisplayAs    = ParseDisplayAs(S);

	return T;
}

// ── Main import ───────────────────────────────────────────────────────────────

void UFP_AffixImporter::ImportAffixes()
{
	const FString ToolsDir   = FPaths::ProjectDir() / TEXT("Tools/MasterDatabase/Affixes");
	const FString AffixesPath = ToolsDir / TEXT("affixes.json");
	const FString PoolsPath   = ToolsDir / TEXT("pools.json");

	TSharedPtr<FJsonObject> AffixesJson = LoadJsonFile(AffixesPath);
	TSharedPtr<FJsonObject> PoolsJson   = LoadJsonFile(PoolsPath);
	if (!AffixesJson || !PoolsJson) return;

	const FString DefDir  = OutputContentPath / TEXT("Definitions");
	const FString PoolDir = OutputContentPath / TEXT("Pools");

	// ── Definitions ───────────────────────────────────────────────────────
	TMap<FName, TObjectPtr<UFP_AffixDefinition>> DefinitionMap;

	for (const auto& Pair : AffixesJson->Values)
	{
		const TSharedPtr<FJsonObject> Obj = Pair.Value->AsObject();
		if (!Obj) continue;

		const FString AssetName   = FString::Printf(TEXT("DA_Affix_%s"), *Pair.Key);
		UFP_AffixDefinition* Def  = GetOrCreateAsset<UFP_AffixDefinition>(DefDir / AssetName, AssetName);

		Def->AffixID = FName(*Pair.Key);

		FString S;
		if (Obj->TryGetStringField(TEXT("Type"),          S)) { Def->Type       = ParseAffixType(S); S.Empty(); }
		if (Obj->TryGetStringField(TEXT("GroupTag"),       S)) { Def->GroupTag   = FName(*S);          S.Empty(); }
		if (Obj->TryGetStringField(TEXT("DisplayTag"),     S)) { Def->DisplayTag = FName(*S);          S.Empty(); }
		if (Obj->TryGetStringField(TEXT("Stat1_Attr"),     S)) { Def->Stat1_Attr = ParseTag(S);        S.Empty(); }
		if (Obj->TryGetStringField(TEXT("Stat2_Attr"),     S)) { Def->Stat2_Attr = ParseTag(S);        S.Empty(); }
		if (Obj->TryGetStringField(TEXT("DisplayFormat"),  S)) { Def->DisplayFormat = S;               S.Empty(); }

		Def->Tiers.Empty();
		const TArray<TSharedPtr<FJsonValue>>* TiersArr = nullptr;
		if (Obj->TryGetArrayField(TEXT("Tiers"), TiersArr))
		{
			for (const TSharedPtr<FJsonValue>& TierVal : *TiersArr)
			{
				if (const TSharedPtr<FJsonObject> TierObj = TierVal->AsObject())
					Def->Tiers.Add(ParseTier(TierObj));
			}
		}

		SaveAsset(Def);
		DefinitionMap.Add(Def->AffixID, Def);
	}

	// ── Pools ─────────────────────────────────────────────────────────────
	int32 PoolCount = 0;

	for (const auto& Pair : PoolsJson->Values)
	{
		const FString AssetName = FString::Printf(TEXT("DA_Pool_%s"), *Pair.Key);
		UFP_AffixPool* Pool     = GetOrCreateAsset<UFP_AffixPool>(PoolDir / AssetName, AssetName);

		Pool->Entries.Empty();
		const TArray<TSharedPtr<FJsonValue>>* EntriesArr = nullptr;
		if (Pair.Value->TryGetArray(EntriesArr))
		{
			for (const TSharedPtr<FJsonValue>& EntryVal : *EntriesArr)
			{
				const TSharedPtr<FJsonObject> EntryObj = EntryVal->AsObject();
				if (!EntryObj) continue;

				FFP_AffixPoolEntry Entry;
				FString S;
				double W = 100.0;

				EntryObj->TryGetStringField(TEXT("GroupTag"),    S); Entry.GroupTag    = FName(*S); S.Empty();
				EntryObj->TryGetStringField(TEXT("AffixID"),     S); Entry.AffixID     = FName(*S); S.Empty();
				EntryObj->TryGetStringField(TEXT("AffinityTag"), S); Entry.AffinityTag = FName(*S);
				EntryObj->TryGetNumberField(TEXT("PoolWeight"),  W); Entry.PoolWeight  = (int32)W;

				Pool->Entries.Add(Entry);
			}
		}

		SaveAsset(Pool);
		PoolCount++;
	}

	// ── Registry ──────────────────────────────────────────────────────────
	const FString RegName = TEXT("DA_AffixRegistry");
	UFP_AffixRegistry* Registry = GetOrCreateAsset<UFP_AffixRegistry>(OutputContentPath / RegName, RegName);
	Registry->Definitions = DefinitionMap;
	SaveAsset(Registry);

	UE_LOG(LogTemp, Log,
		TEXT("FP_AffixImporter: imported %d definitions, %d pools -> %s"),
		DefinitionMap.Num(), PoolCount, *OutputContentPath);
}

#endif // WITH_EDITOR
