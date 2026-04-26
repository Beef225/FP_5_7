// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "FP_Leaf.h"
#include "FP_Leaf_Image.generated.h"

class UImage;
class USizeBox;

/**
 * Leaf widget that displays an item's icon texture.
 * Populated by FFP_ImageFragment::Assimilate when an item is hovered.
 */
UCLASS()
class FP_5_7_API UFP_Leaf_Image : public UFP_Leaf
{
	GENERATED_BODY()

public:

	void SetImage(UTexture2D* Texture) const;
	void SetBoxSize(const FVector2D& Size) const;
	void SetImageSize(const FVector2D& Size) const;
	FVector2D GetImageSize() const;

private:

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> Image_Icon;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<USizeBox> SizeBox_Icon;
};
