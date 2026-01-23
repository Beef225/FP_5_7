// Copyright JG


#include "FP_AssetManager.h"
#include "AbilitySystemGlobals.h"
#include "FP_GameplayTags.h"

UFP_AssetManager& UFP_AssetManager::Get()
{
	check(GEngine);
	UFP_AssetManager* FP_AssetManager = Cast<UFP_AssetManager>(GEngine->AssetManager);
	return *FP_AssetManager;
}

void UFP_AssetManager::StartInitialLoading()
{
	Super::StartInitialLoading();

	FFP_GameplayTags::InitializeGameplayTags();


	UAbilitySystemGlobals::Get().InitGlobalData();


}
