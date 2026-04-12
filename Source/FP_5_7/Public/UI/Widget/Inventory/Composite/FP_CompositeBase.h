// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "GameplayTagContainer.h"
#include "FP_CompositeBase.generated.h"

/**
 * Abstract base class for the Composite widget pattern.
 * Both composites (nodes) and leaves derive from this.
 * FragmentTag identifies which item fragment this node represents.
 */
UCLASS()
class FP_5_7_API UFP_CompositeBase : public UUserWidget
{
	GENERATED_BODY()

public:

	using FuncType = TFunction<void(UFP_CompositeBase*)>;

	FGameplayTag GetFragmentTag() const { return FragmentTag; }
	void SetFragmentTag(const FGameplayTag& Tag) { FragmentTag = Tag; }

	virtual void Collapse();
	void Expand();

	virtual void ApplyFunction(FuncType Function) {}

private:

	UPROPERTY(EditAnywhere, Category = "Inventory")
	FGameplayTag FragmentTag;
};
