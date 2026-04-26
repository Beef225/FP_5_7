// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "FP_Leaf.h"
#include "FP_Leaf_Text.generated.h"

class UTextBlock;

/**
 * Leaf widget that displays a text string (e.g. item name, description line).
 * FontSize is configurable per-widget in the Blueprint Details panel.
 * Populated by FFP_TextFragment::Assimilate when an item is hovered.
 */
UCLASS()
class FP_5_7_API UFP_Leaf_Text : public UFP_Leaf
{
	GENERATED_BODY()

public:

	void SetText(const FText& Text) const;
	virtual void NativePreConstruct() override;

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> Text_LeafText;

	UPROPERTY(EditAnywhere, Category = "Inventory")
	int32 FontSize{ 12 };
};
