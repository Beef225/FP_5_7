// Copyright JG

#include "SkillTree/FP_SkillTreeImporter.h"

#if WITH_EDITOR

#include "SkillTree/FP_SkillTreeNodeData.h"
#include "SkillTree/FP_SkillTreeNodeEffect.h"
#include "UI/Widget/SkillTree/FP_SkillTreeWidget.h"
#include "Libraries/FP_EnumDefs.h"
#include "StructUtils/InstancedStruct.h"
#include "DesktopPlatformModule.h"
#include "IDesktopPlatform.h"
#include "Misc/FileHelper.h"
#include "Misc/Paths.h"
#include "Serialization/JsonReader.h"
#include "Serialization/JsonSerializer.h"
#include "Dom/JsonObject.h"
#include "GameplayTagsManager.h"
#include "AssetToolsModule.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "Factories/DataAssetFactory.h"
#include "Engine/Texture2D.h"
#include "Framework/Notifications/NotificationManager.h"
#include "Widgets/Notifications/SNotificationList.h"

static UFP_SkillTreeNodeData* GetOrCreateNodeAsset(const FString& AssetName, const FString& OutputPath, bool& bOutCreated)
{
	bOutCreated = false;

	const FString PackagePath = OutputPath / AssetName;
	if (UObject* Existing = StaticLoadObject(UFP_SkillTreeNodeData::StaticClass(), nullptr, *PackagePath))
		return Cast<UFP_SkillTreeNodeData>(Existing);

	IAssetTools& AssetTools = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools").Get();
	UDataAssetFactory* Factory = NewObject<UDataAssetFactory>();
	Factory->DataAssetClass = UFP_SkillTreeNodeData::StaticClass();

	UObject* NewAsset = AssetTools.CreateAsset(AssetName, OutputPath, UFP_SkillTreeNodeData::StaticClass(), Factory);
	bOutCreated = (NewAsset != nullptr);
	return Cast<UFP_SkillTreeNodeData>(NewAsset);
}

static void ApplyNodeJSON(UFP_SkillTreeNodeData* Asset, const TSharedPtr<FJsonObject>& NodeJson)
{
	if (!Asset) return;

	FString TagStr;
	if (NodeJson->TryGetStringField(TEXT("NodeTag"), TagStr))
		Asset->NodeTag = UGameplayTagsManager::Get().RequestGameplayTag(FName(*TagStr), false);

	FString TypeStr;
	if (NodeJson->TryGetStringField(TEXT("NodeType"), TypeStr))
	{
		if      (TypeStr == TEXT("Major"))   Asset->NodeType = ESkillTreeNodeType::Major;
		else if (TypeStr == TEXT("Mastery")) Asset->NodeType = ESkillTreeNodeType::Mastery;
		else                                 Asset->NodeType = ESkillTreeNodeType::Minor;
	}

	bool bStarter = false;
	if (NodeJson->TryGetBoolField(TEXT("IsStartNode"), bStarter))
		Asset->bIsStarterNode = bStarter;

	FString NameStr, DescStr;
	if (NodeJson->TryGetStringField(TEXT("NodeName"), NameStr))
		Asset->NodeName = FText::FromString(NameStr);
	if (NodeJson->TryGetStringField(TEXT("NodeDescription"), DescStr))
		Asset->NodeDescription = FText::FromString(DescStr);

	const TSharedPtr<FJsonObject>* PosObj;
	if (NodeJson->TryGetObjectField(TEXT("NodePosition"), PosObj))
	{
		double X = 0.0, Y = 0.0;
		(*PosObj)->TryGetNumberField(TEXT("X"), X);
		(*PosObj)->TryGetNumberField(TEXT("Y"), Y);
		Asset->NodePosition = FVector2D(static_cast<float>(X), static_cast<float>(Y));
	}

	const TArray<TSharedPtr<FJsonValue>>* ConnArray;
	if (NodeJson->TryGetArrayField(TEXT("ConnectedNodeTags"), ConnArray))
	{
		Asset->ConnectedNodeTags.Empty();
		for (const TSharedPtr<FJsonValue>& Val : *ConnArray)
		{
			const FString ConnTag = Val->AsString();
			if (!ConnTag.IsEmpty())
				Asset->ConnectedNodeTags.Add(UGameplayTagsManager::Get().RequestGameplayTag(FName(*ConnTag), false));
		}
	}

	FString IconPath;
	if (NodeJson->TryGetStringField(TEXT("NodeIcon"), IconPath) && !IconPath.IsEmpty())
	{
		if (UTexture2D* Icon = LoadObject<UTexture2D>(nullptr, *IconPath))
			Asset->NodeIcon = Icon;
	}

	// Effects
	const TArray<TSharedPtr<FJsonValue>>* EffectsArray;
	if (NodeJson->TryGetArrayField(TEXT("Effects"), EffectsArray))
	{
		Asset->Effects.Empty();
		for (const TSharedPtr<FJsonValue>& EffVal : *EffectsArray)
		{
			const TSharedPtr<FJsonObject> EffJson = EffVal->AsObject();
			if (!EffJson.IsValid()) continue;

			FString EffectType;
			if (!EffJson->TryGetStringField(TEXT("EffectType"), EffectType)) continue;

			if (EffectType == TEXT("Stat Mod"))
			{
				TInstancedStruct<FFP_SkillTreeNodeEffect> Entry;
				Entry.InitializeAs<FFP_NodeEffect_StatModifier>();
				FFP_NodeEffect_StatModifier& S = Entry.GetMutable<FFP_NodeEffect_StatModifier>();

				FString AttrTag;
				if (EffJson->TryGetStringField(TEXT("EffectStatModAttributeType"), AttrTag))
					S.AttributeTag = UGameplayTagsManager::Get().RequestGameplayTag(FName(*AttrTag), false);

				FString ModOp;
				if (EffJson->TryGetStringField(TEXT("EffectStatModifierOp"), ModOp))
					S.ModifierOp = (ModOp == TEXT("Multiply")) ? EGameplayModOp::Multiplicitive : EGameplayModOp::Additive;

				double Mag = 0.0;
				if (EffJson->TryGetNumberField(TEXT("EffectStatMagnitude"), Mag))
					S.Magnitude = static_cast<float>(Mag);

				Asset->Effects.Add(MoveTemp(Entry));
			}
			else if (EffectType == TEXT("Grant Skill"))
			{
				TInstancedStruct<FFP_SkillTreeNodeEffect> Entry;
				Entry.InitializeAs<FFP_NodeEffect_GrantSkill>();
				FFP_NodeEffect_GrantSkill& G = Entry.GetMutable<FFP_NodeEffect_GrantSkill>();

				FString SkillTag;
				if (EffJson->TryGetStringField(TEXT("EffectSkillTag"), SkillTag))
					G.SkillTag = UGameplayTagsManager::Get().RequestGameplayTag(FName(*SkillTag), false);

				Asset->Effects.Add(MoveTemp(Entry));
			}
			else if (EffectType == TEXT("Effect Flag"))
			{
				TInstancedStruct<FFP_SkillTreeNodeEffect> Entry;
				Entry.InitializeAs<FFP_NodeEffect_Flag>();
				FFP_NodeEffect_Flag& F = Entry.GetMutable<FFP_NodeEffect_Flag>();

				FString FlagTag;
				if (EffJson->TryGetStringField(TEXT("EffectFlagTag"), FlagTag))
					F.FlagTag = UGameplayTagsManager::Get().RequestGameplayTag(FName(*FlagTag), false);

				Asset->Effects.Add(MoveTemp(Entry));
			}
			else
			{
				UE_LOG(LogTemp, Warning, TEXT("SkillTreeImporter: unknown EffectType '%s' on node — skipped"), *EffectType);
			}
		}
	}
}

void UFP_SkillTreeImporter::ImportFromJSON()
{
	IDesktopPlatform* DesktopPlatform = FDesktopPlatformModule::Get();
	if (!DesktopPlatform)
	{
		UE_LOG(LogTemp, Error, TEXT("SkillTreeImporter: IDesktopPlatform unavailable"));
		return;
	}

	TArray<FString> OutFiles;
	const FString DefaultPath = FPaths::ConvertRelativePathToFull(FPaths::ProjectContentDir());
	const bool bOpened = DesktopPlatform->OpenFileDialog(
		nullptr,
		TEXT("Select Skill Tree JSON"),
		DefaultPath,
		TEXT(""),
		TEXT("JSON files (*.json)|*.json"),
		EFileDialogFlags::None,
		OutFiles);

	if (!bOpened || OutFiles.IsEmpty()) return;

	FString JsonText;
	if (!FFileHelper::LoadFileToString(JsonText, *OutFiles[0]))
	{
		UE_LOG(LogTemp, Error, TEXT("SkillTreeImporter: failed to read %s"), *OutFiles[0]);
		return;
	}

	TSharedPtr<FJsonObject> Root;
	const TSharedRef<TJsonReader<>> Reader = TJsonReaderFactory<>::Create(JsonText);
	if (!FJsonSerializer::Deserialize(Reader, Root) || !Root.IsValid())
	{
		UE_LOG(LogTemp, Error, TEXT("SkillTreeImporter: JSON parse failed"));
		return;
	}

	const TArray<TSharedPtr<FJsonValue>>* NodesArray;
	if (!Root->TryGetArrayField(TEXT("nodes"), NodesArray))
	{
		UE_LOG(LogTemp, Error, TEXT("SkillTreeImporter: no 'nodes' array in JSON"));
		return;
	}

	if (NodeAssetOutputPath.IsEmpty())
	{
		UE_LOG(LogTemp, Error, TEXT("SkillTreeImporter: NodeAssetOutputPath is empty"));
		return;
	}

	int32 Created = 0, Updated = 0;
	for (const TSharedPtr<FJsonValue>& Entry : *NodesArray)
	{
		const TSharedPtr<FJsonObject> NodeJson = Entry->AsObject();
		if (!NodeJson.IsValid()) continue;

		FString TagStr;
		if (!NodeJson->TryGetStringField(TEXT("NodeTag"), TagStr) || TagStr.IsEmpty()) continue;

		FString AssetName = FString::Printf(TEXT("DA_SkillNode_%s"), *TagStr.Replace(TEXT("."), TEXT("_")));

		bool bWasCreated = false;
		UFP_SkillTreeNodeData* Asset = GetOrCreateNodeAsset(AssetName, NodeAssetOutputPath, bWasCreated);
		if (!Asset) continue;

		Modify();
		ApplyNodeJSON(Asset, NodeJson);
		Asset->MarkPackageDirty();

		bWasCreated ? ++Created : ++Updated;
	}

	// Write created assets into the target widget CDO so NodeDataAssets is populated automatically
	if (TargetWidgetClass)
	{
		UFP_SkillTreeWidget* CDO = Cast<UFP_SkillTreeWidget>(TargetWidgetClass->GetDefaultObject());
		if (CDO)
		{
			CDO->Modify();
			CDO->NodeDataAssets.Empty();

			FAssetRegistryModule& AR = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
			TArray<FAssetData> Found;
			AR.Get().GetAssetsByPath(FName(*NodeAssetOutputPath), Found, /*bRecursive=*/true);
			for (const FAssetData& AD : Found)
			{
				if (UFP_SkillTreeNodeData* Asset = Cast<UFP_SkillTreeNodeData>(AD.GetAsset()))
					CDO->NodeDataAssets.Add(Asset);
			}

			CDO->MarkPackageDirty();
			UE_LOG(LogTemp, Log, TEXT("SkillTreeImporter: wrote %d assets to %s CDO"),
				CDO->NodeDataAssets.Num(), *TargetWidgetClass->GetName());
		}
	}

	FNotificationInfo Info(FText::Format(
		NSLOCTEXT("SkillTree", "ImportDone", "Skill Tree import complete: {0} created, {1} updated"),
		FText::AsNumber(Created), FText::AsNumber(Updated)));
	Info.ExpireDuration = 4.f;
	FSlateNotificationManager::Get().AddNotification(Info);

	UE_LOG(LogTemp, Log, TEXT("SkillTreeImporter: %d created, %d updated"), Created, Updated);
}

#endif
