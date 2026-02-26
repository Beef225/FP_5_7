// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UI/WidgetController/FP_CharacterMenuWidgetController.h"
#include "FP_AbilitySystemLibrary.generated.h"

class UAbilitySystemComponent;
class UFP_OverlayWidgetController;
class UFP_InventoryWidgetController;
class UCharacterMenuWidgetController;
class UFP_SkillLibrary;
/**
 * 
 */
UCLASS()
class FP_5_7_API UFP_AbilitySystemLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
public: 
	UFUNCTION(BlueprintPure, Category="FP_AbilitySystemLibrary|WidgetController")
	static UFP_OverlayWidgetController* GetOverlayWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category="FP_AbilitySystemLibrary|WidgetController")
	static UFP_CharacterMenuWidgetController* GetCharacterWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category="FP_AbilitySystemLibrary|WidgetController")
	static UFP_InventoryWidgetController* GetInventoryWidgetController(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintCallable, Category="FP_AbilitySystemLibrary|CharacterClassDefaults")
	static void InitializeDefaultAttributes(const UObject* WorldContextObject, ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC);
	
	UFUNCTION(BlueprintCallable, Category="FP_AbilitySystemLibrary|CharacterClassDefaults")
	static void GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass);
	
	UFUNCTION(BlueprintCallable, Category = "FP_AbilitySystemLibrary|CharacterClassDefaults")
	static UCharacterClassInfo* GetCharacterClassInfo(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category="FP_AbilitySystemLibrary|Abilities")
	static UFP_SkillLibrary* GetSkillLibrary(const UObject* WorldContextObject);
	
	UFUNCTION(BlueprintPure, Category = "FP_AbilitySystemLibrary|GameplayEffects")
	static bool IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle);
	
	UFUNCTION(BlueprintPure, Category = "FP_AbilitySystemLibrary|GameplayEffects")
	static bool IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle);
	
	UFUNCTION(BlueprintPure, Category = "FP_AbilitySystemLibrary|GameplayEffects")
	static bool IsDodgedHit(const FGameplayEffectContextHandle& EffectContextHandle);
	
	UFUNCTION(BlueprintPure, Category = "FP_AbilitySystemLibrary|GameplayEffects")
	static void SetIsBlockedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit);
	
	UFUNCTION(BlueprintPure, Category = "FP_AbilitySystemLibrary|GameplayEffects")
	static void SetIsCriticalHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsCriticalHit);
	
	UFUNCTION(BlueprintPure, Category = "FP_AbilitySystemLibrary|GameplayEffects")
	static void SetIsDodgedHit(UPARAM(ref) FGameplayEffectContextHandle& EffectContextHandle, bool bInIsDodgedHit);
	
	UFUNCTION(BlueprintCallable, Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static void GetLivePlayersWithinRadius(const UObject* WorldContextObject, TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius, const FVector& SphereOrigin);
	
	UFUNCTION(BlueprintPure, Category = "AuraAbilitySystemLibrary|GameplayMechanics")
	static bool IsNotFriend(AActor* FirstActor, AActor* SecondActor);
};
