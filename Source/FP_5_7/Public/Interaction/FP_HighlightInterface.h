// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "FP_HighlightInterface.generated.h"


UINTERFACE(MinimalAPI, BlueprintType)
class UFP_HighlightInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * Implemented by any actor that can be highlighted when the player hovers over it.
 *
 * How colour is chosen:
 *   Each implementing actor sets its own custom depth stencil value using the
 *   CUSTOM_DEPTH_* constants defined in FP_5_7.h (RED=250, BLUE=251, TAN=252).
 *   The post-process material reads that stencil value and maps it to a colour.
 *   So the actor itself decides its highlight colour — the controller just calls
 *   HighlightActor/UnHighlightActor without needing to know the colour.
 *
 * To give an actor a highlight:
 *   1. Implement this interface on the actor (C++ or Blueprint).
 *   2. In HighlightActor_Implementation, call SetRenderCustomDepth(true) and
 *      SetCustomDepthStencilValue(CUSTOM_DEPTH_*) on the relevant mesh(es).
 *   3. In UnHighlightActor_Implementation, call SetRenderCustomDepth(false).
 */
class FP_5_7_API IFP_HighlightInterface
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintNativeEvent)
	void HighlightActor();

	UFUNCTION(BlueprintNativeEvent)
	void UnHighlightActor();
};
