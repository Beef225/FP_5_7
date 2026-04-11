// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FP_ItemDescription.generated.h"

class USizeBox;

/**
 * Widget that displays a tooltip-style description for an inventory item.
 * Shown on hover with a short timer delay; positioned to follow the cursor.
 */
UCLASS()
class FP_5_7_API UFP_ItemDescription : public UUserWidget
{
	GENERATED_BODY()

public:

	/** Returns the current desired size of the SizeBox wrapping the description content. */
	FVector2D GetBoxSize() const;

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SizeBox;
};
