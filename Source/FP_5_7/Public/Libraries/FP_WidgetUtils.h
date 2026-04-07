// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FP_WidgetUtils.generated.h"

UCLASS()
class FP_5_7_API UFP_WidgetUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/** Converts a 2D tile coordinate to a flat 1D index. */
	UFUNCTION(BlueprintPure, Category = "Inventory|Utils")
	static int32 GetIndexFromPosition(const FIntPoint& Position, int32 Columns);

	/** Inverse of GetIndexFromPosition — converts a flat 1D index back to a 2D tile coordinate. */
	UFUNCTION(BlueprintPure, Category = "Inventory|Utils")
	static FIntPoint GetPositionFromIndex(int32 Index, int32 Columns);
};
