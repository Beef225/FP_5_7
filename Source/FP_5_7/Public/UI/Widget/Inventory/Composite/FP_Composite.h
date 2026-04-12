// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "FP_CompositeBase.h"
#include "FP_Composite.generated.h"

/**
 * Composite node. On init, discovers all UFP_CompositeBase children in the widget tree,
 * collapses them, and stores them. ApplyFunction and Collapse propagate to all children.
 */
UCLASS()
class FP_5_7_API UFP_Composite : public UFP_CompositeBase
{
	GENERATED_BODY()

public:

	virtual void NativeOnInitialized() override;
	virtual void ApplyFunction(FuncType Function) override;
	virtual void Collapse() override;

private:

	UPROPERTY()
	TArray<TObjectPtr<UFP_CompositeBase>> Children;
};
