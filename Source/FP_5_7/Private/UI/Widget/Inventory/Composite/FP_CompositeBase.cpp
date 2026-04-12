// Copyright JG

#include "UI/Widget/Inventory/Composite/FP_CompositeBase.h"

void UFP_CompositeBase::Collapse()
{
	SetVisibility(ESlateVisibility::Collapsed);
}

void UFP_CompositeBase::Expand()
{
	SetVisibility(ESlateVisibility::Visible);
}
