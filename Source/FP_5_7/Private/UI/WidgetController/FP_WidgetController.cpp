// Copyright JG


#include "UI/WidgetController/FP_WidgetController.h"

void UFP_WidgetController::SetWidgetControllerParams(const FWidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}

void UFP_WidgetController::BroadcastInitialValues()
{
}

void UFP_WidgetController::BindCallbacksToDependencies()
{
}
