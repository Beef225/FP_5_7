// Copyright JG

#include "UI/Widget/Items/FP_ItemDescription.h"
#include "Components/SizeBox.h"

FVector2D UFP_ItemDescription::GetBoxSize() const
{
	return SizeBox->GetDesiredSize();
}

void UFP_ItemDescription::SetFixedWidth(float Width)
{
	SizeBox->SetWidthOverride(Width);
}

void UFP_ItemDescription::UpdateWidth(float MinWidth)
{
	SizeBox->ClearWidthOverride();
	ForceLayoutPrepass();
	const float Natural = SizeBox->GetDesiredSize().X;
	SizeBox->SetWidthOverride(FMath::Max(MinWidth, Natural));
}
