// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "FP_Leaf.h"
#include "FP_Leaf_LabeledValue.generated.h"

class UTextBlock;

/**
 * Leaf widget displaying a label-value pair (e.g. "Damage  14.5").
 * Both text blocks have individually configurable font sizes.
 * Populated by FFP_LabeledNumberFragment::Assimilate when an item is hovered.
 */
UCLASS()
class FP_5_7_API UFP_Leaf_LabeledValue : public UFP_Leaf
{
	GENERATED_BODY()

public:

	/** Sets the label text. If bCollapse is true, collapses the label instead. */
	void SetText_Label(const FText& Text, bool bCollapse) const;

	/** Sets the value text. If bCollapse is true, collapses the value instead. */
	void SetText_Value(const FText& Text, bool bCollapse) const;

	virtual void NativePreConstruct() override;

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Label;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_Value;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 FontSize_Label{ 12 };

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 FontSize_Value{ 18 };
};
