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

	/** Converts a widget's local top-left to viewport space coordinates. */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Utils")
	static FVector2D GetWidgetPosition(UWidget* Widget);

	/** Returns the local size of a widget from its cached geometry. */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Utils")
	static FVector2D GetWidgetSize(UWidget* Widget);

	/** Returns true if MousePos falls within the rectangle defined by BoundaryPos and WidgetSize. */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Utils")
	static bool IsWithinBounds(const FVector2D& BoundaryPos, const FVector2D& WidgetSize, const FVector2D& MousePos);

	/**
	 * Returns a position clamped so that a widget of WidgetSize stays fully inside a boundary of size Boundary.
	 * MousePos is the desired position (e.g. cursor location in viewport space).
	 */
	UFUNCTION(BlueprintCallable, Category = "Inventory|Utils")
	static FVector2D GetClampedWidgetPosition(const FVector2D& Boundary, const FVector2D& WidgetSize, const FVector2D& MousePos);
};
