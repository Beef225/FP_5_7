// Copyright JG


#include "UI/Widget/FP_UserWidget.h"


void UFP_UserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
