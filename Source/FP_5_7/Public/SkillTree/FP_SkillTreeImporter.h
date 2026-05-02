// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "FP_SkillTreeImporter.generated.h"

class UFP_SkillTreeWidget;

/**
 * Editor-only data asset for importing skill tree nodes from JSON.
 *
 * Workflow:
 *   1. Right-click Content Browser > Miscellaneous > Data Asset > FP_SkillTreeImporter.
 *   2. Set NodeAssetOutputPath to the folder for this tree's node data assets.
 *   3. Set TargetWidgetClass to your BP subclass of UFP_SkillTreeWidget.
 *   4. Click "Import From JSON", select the SVG-exported JSON.
 *      Node data assets are created and NodeDataAssets on the widget is populated automatically.
 */
UCLASS()
class FP_5_7_API UFP_SkillTreeImporter : public UDataAsset
{
	GENERATED_BODY()

public:
	/** Content-browser path where UFP_SkillTreeNodeData assets will be created/updated. */
	UPROPERTY(EditDefaultsOnly, Category="Skill Tree Import")
	FString NodeAssetOutputPath = TEXT("/Game/SkillTrees/Data/Nodes");

	/** Widget BP subclass whose NodeDataAssets will be populated after import. */
	UPROPERTY(EditDefaultsOnly, Category="Skill Tree Import")
	TSubclassOf<UFP_SkillTreeWidget> TargetWidgetClass;

#if WITH_EDITOR
	UFUNCTION(CallInEditor, Category="Skill Tree Import")
	void ImportFromJSON();
#endif
};
