// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "FP_CompositeBase.h"
#include "FP_Leaf.generated.h"

/**
 * Leaf node in the composite pattern. Has no children.
 * ApplyFunction executes the given function on itself.
 */
UCLASS()
class FP_5_7_API UFP_Leaf : public UFP_CompositeBase
{
	GENERATED_BODY()

public:

	virtual void ApplyFunction(FuncType Function) override;
};
