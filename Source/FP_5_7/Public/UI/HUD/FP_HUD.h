// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/WidgetController/FP_OverlayWidgetController.h"
#include "FP_HUD.generated.h"


class UAttributeSet;
class UAbilitySystemComponent;
class UFP_WidgetController;
class UFP_UserWidget;
struct FWidgetControllerParams;


/**
 * 
 */
UCLASS()
class FP_5_7_API AFP_HUD : public AHUD
{
	GENERATED_BODY()
	
public:
	
	UPROPERTY()
	TObjectPtr<UFP_UserWidget> OverlayWidget;
	
	UFP_OverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);
	
protected:
	
	
private:
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UFP_UserWidget> OverlayWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UFP_OverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UFP_OverlayWidgetController> OverlayWidgetControllerClass;
};
