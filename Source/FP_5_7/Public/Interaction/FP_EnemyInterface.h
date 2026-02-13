// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FP_EnemyInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UFP_EnemyInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FP_5_7_API IFP_EnemyInterface
{
	GENERATED_BODY()
public:
	
	virtual void HighlightActor() = 0;
	virtual void UnHighlightActor() = 0;

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	void SetCombatTarget(AActor* InCombatTarget);

	UFUNCTION(BlueprintCallable, BlueprintNativeEvent)
	AActor* GetCombatTarget() const;
};
