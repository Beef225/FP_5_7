// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Engine/AssetManager.h"
#include "FP_AssetManager.generated.h"

/**
 * 
 */
UCLASS()
class FP_5_7_API UFP_AssetManager : public UAssetManager
{
	GENERATED_BODY()
	
public:
	
	static UFP_AssetManager& Get();
	
protected:
	virtual void StartInitialLoading() override;
};
