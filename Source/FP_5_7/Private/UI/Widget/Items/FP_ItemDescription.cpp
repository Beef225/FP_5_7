// Copyright JG

#include "UI/Widget/Items/FP_ItemDescription.h"
#include "Components/SizeBox.h"

FVector2D UFP_ItemDescription::GetBoxSize() const
{
	return SizeBox->GetDesiredSize();
}
