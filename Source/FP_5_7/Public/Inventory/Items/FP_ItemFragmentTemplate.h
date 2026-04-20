// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "Inventory/Items/Fragments/FP_ItemFragment.h"
#include "StructUtils/InstancedStruct.h"
#include "FP_ItemFragmentTemplate.generated.h"

/**
 * Data asset that defines a reusable fragment scaffold for a category of items.
 *
 * Configure one template per item archetype (e.g. DA_Template_Equipment_Base,
 * DA_Template_Weapon). Each entry is a fully configured fragment with its
 * FragmentTag already set.
 *
 * On AFP_ItemActor, pick this template and click "Apply Template" in Class Defaults.
 * Fragments are added by tag — any fragment whose tag is already present in the
 * manifest is left untouched, so hand-configured data is never overwritten.
 */
UCLASS(BlueprintType)
class FP_5_7_API UFP_ItemFragmentTemplate : public UDataAsset
{
	GENERATED_BODY()

public:

	const TArray<TInstancedStruct<FFP_ItemFragment>>& GetFragments() const { return Fragments; }

private:

	UPROPERTY(EditAnywhere, Category="Template", meta=(ExcludeBaseStruct))
	TArray<TInstancedStruct<FFP_ItemFragment>> Fragments;
};
