// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FP_PlayerInterface.generated.h"

// This class does not need to be modified.
UINTERFACE()
class UFP_PlayerInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 
 */
class FP_5_7_API IFP_PlayerInterface
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddToXP(int32 InXP);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetXP() const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 FindLevelForXP(int32 InXP) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	int32 GetAttributePointsReward(int32 Level) const;

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddToPlayerLevel(int32 InLevel);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void AddToAttributePoints(int32 InPoints);

	UFUNCTION(BlueprintNativeEvent, BlueprintCallable)
	void LevelUp();
};
