// Copyright JG

#include "Libraries/FP_WidgetUtils.h"

int32 UFP_WidgetUtils::GetIndexFromPosition(const FIntPoint& Position, const int32 Columns)
{
	return Position.X + Position.Y * Columns;
}
