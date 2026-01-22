// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FP_AbilitySystemLibrary.generated.h"

class UFP_OverlayWidgetController;
class UFP_InventoryWidgetController;
class UCharacterMenuWidgetController;
/**
 * 
 */
UCLASS()
class FP_5_7_API UFP_AbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|WidgetController")
	static UFP_OverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|WidgetController")
	static UFP_CharacterMenuWidgetController* GetCharacterWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|WidgetController")
	static UFP_InventoryWidgetController* GetInventoryWidgetController(const UObject* WorldContextObject);
	
	
};
