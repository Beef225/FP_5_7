// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "FP_GridSlot.generated.h"

class UImage;

/**
 * A single tile in the inventory grid. Bound to an Image widget in Blueprint.
 */
UCLASS()
class FP_5_7_API UFP_GridSlot : public UUserWidget
{
	GENERATED_BODY()

public:

	void SetTileIndex(int32 Index) { TileIndex = Index; }
	int32 GetTileIndex() const { return TileIndex; }

private:

	int32 TileIndex = 0;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_GridSlot;
};
