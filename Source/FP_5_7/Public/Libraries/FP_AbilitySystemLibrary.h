// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FP_AbilitySystemLibrary.generated.h"

class UAbilitySystemComponent;
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
public: 
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|WidgetController")
	static UFP_OverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|WidgetController")
	static UFP_CharacterMenuWidgetController* GetCharacterWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category="AuraAbilitySystemLibrary|WidgetController")
	static UFP_InventoryWidgetController* GetInventoryWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category="AuraAbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);
	
	
};
