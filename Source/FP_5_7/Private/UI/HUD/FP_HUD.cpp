// Copyright JG


#include "UI/HUD/FP_HUD.h"
#include "UI/Widget/FP_UserWidget.h"
#include "UI/WidgetController/FP_OverlayWidgetController.h"
#include "UI/WidgetController/FP_CharacterMenuWidgetController.h"
#include "UI/WidgetController/FP_InventoryWidgetController.h"

UFP_OverlayWidgetController* AFP_HUD::GetOverlayWidgetController(const FWidgetControllerParams& WCParams)
{
	if (OverlayWidgetController == nullptr)
	{
		OverlayWidgetController = NewObject<UFP_OverlayWidgetController>(this, OverlayWidgetControllerClass);
		OverlayWidgetController->SetWidgetControllerParams(WCParams);
		OverlayWidgetController->BindCallbacksToDependencies();
	}
	return OverlayWidgetController;
}

UFP_CharacterMenuWidgetController* AFP_HUD::GetCharacterMenuWidgetController(const FWidgetControllerParams& WCParams)
{
	if (CharacterWidgetController == nullptr)
	{
		CharacterWidgetController = NewObject<UFP_CharacterMenuWidgetController>(this, CharacterWidgetControllerClass);
		CharacterWidgetController->SetWidgetControllerParams(WCParams);
		CharacterWidgetController->BindCallbacksToDependencies();
	}
	return CharacterWidgetController;
}

UFP_InventoryWidgetController* AFP_HUD::GetInventoryWidgetController(const FWidgetControllerParams& WCParams)
{
	if (InventoryWidgetController = nullptr)
	{
		InventoryWidgetController = NewObject<UFP_InventoryWidgetController>(this, InventoryWidgetControllerClass);
		InventoryWidgetController->SetWidgetControllerParams(WCParams);
		InventoryWidgetController->BindCallbacksToDependencies();		
	}
	return InventoryWidgetController;
}

void AFP_HUD::InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS)
{
	checkf(OverlayWidgetClass, TEXT("Overlay Widget Class uninitialized, please fill out BP_FP_HUD"));
	checkf(OverlayWidgetControllerClass, TEXT("Overlay Widget Controller Class uninitialized, please fill out BP_FP_HUD"));
	
	UUserWidget* Widget = CreateWidget<UUserWidget>(GetWorld(), OverlayWidgetClass);
	
	OverlayWidget = Cast<UFP_UserWidget>(Widget);
	
	const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
	UFP_OverlayWidgetController* WidgetController = GetOverlayWidgetController(WidgetControllerParams);

	OverlayWidget->SetWidgetController(WidgetController);
	WidgetController->BroadcastInitialValues();
	Widget->AddToViewport();
	
}


