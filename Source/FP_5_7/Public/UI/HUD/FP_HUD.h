// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/HUD.h"
#include "UI/WidgetController/FP_OverlayWidgetController.h"
#include "FP_HUD.generated.h"


class UFP_CharacterMenuWidgetController;
class UFP_InventoryWidgetController;
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
		
	UFP_OverlayWidgetController* GetOverlayWidgetController(const FWidgetControllerParams& WCParams);
	UFP_CharacterMenuWidgetController* GetCharacterMenuWidgetController(const FWidgetControllerParams& WCParams);
	UFP_InventoryWidgetController* GetInventoryWidgetController(const FWidgetControllerParams& WCParams);

	void InitOverlay(APlayerController* PC, APlayerState* PS, UAbilitySystemComponent* ASC, UAttributeSet* AS);
	
protected:
	
	
private:
	UPROPERTY()
	TObjectPtr<UFP_UserWidget> OverlayWidget;
	
	UPROPERTY(EditAnywhere)
	TSubclassOf<UFP_UserWidget> OverlayWidgetClass;
	
	UPROPERTY()
	TObjectPtr<UFP_OverlayWidgetController> OverlayWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UFP_OverlayWidgetController> OverlayWidgetControllerClass;
	
	UPROPERTY()
	TObjectPtr<UFP_CharacterMenuWidgetController> CharacterWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UFP_CharacterMenuWidgetController> CharacterWidgetControllerClass;
	
	UPROPERTY()
	TObjectPtr<UFP_InventoryWidgetController> InventoryWidgetController;

	UPROPERTY(EditAnywhere)
	TSubclassOf<UFP_InventoryWidgetController> InventoryWidgetControllerClass;
};
