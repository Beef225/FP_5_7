// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "AbilitySystem/Data/CharacterClassInfo.h"
#include "Inventory/Items/FP_ItemComponent.h"
#include "Inventory/Loot/Data/FP_LootTiers.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "Libraries/FP_EnumDefs.h"
#include "Libraries/FP_WidgetUtils.h"
#include "UI/WidgetController/FP_CharacterMenuWidgetController.h"
#include "FP_AbilitySystemLibrary.generated.h"

class UAbilitySystemComponent;
class UFP_HoverItem;
class UFP_OverlayWidgetController;
class UFP_InventoryWidgetController;
class UCharacterMenuWidgetController;
class UFP_SkillLibrary;
class UFP_InventoryComponent;
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

	UFUNCTION(BlueprintPure, Category = "FP_AbilitySystemLibrary|XP")
	static int32 GetXPReward(const UObject* WorldContextObject, int32 BaseXP, int32 CharacterLevel);

	UFUNCTION(BlueprintCallable, Category = "FP_AbilitySystemLibrary|LootTiers", meta = (WorldContext = "WorldContextObject"))
	static UFP_LootTiers* GetLootTiers(const UObject* WorldContextObject);

	/**
	 * Rolls the global loot table plus any extra tables assigned to the enemy,
	 * and returns the combined list of items to spawn.
	 */
	UFUNCTION(BlueprintCallable, Category = "FP_AbilitySystemLibrary|LootTiers", meta = (WorldContext = "WorldContextObject"))
	static TArray<FLootItem> GetLootItemsForEnemy(const UObject* WorldContextObject, const TArray<UFP_LootTiers*>& ExtraLootTiers);

	/** Returns the UFP_InventoryComponent on the given PlayerController, or nullptr. */
	UFUNCTION(BlueprintCallable, Category = "FP_AbilitySystemLibrary|Inventory")
	static UFP_InventoryComponent* GetInventoryComponent(const APlayerController* PlayerController);

	/** Reads the item category directly from the ItemComponent's manifest. Returns None if invalid. */
	UFUNCTION(BlueprintCallable, Category = "FP_AbilitySystemLibrary|Inventory")
	static EItemCategory GetItemCategoryFromItemComp(UFP_ItemComponent* ItemComp);

	/** Returns the hover item currently held in the player's inventory UI, or nullptr. */
	static UFP_HoverItem* GetHoverItem(APlayerController* PC);

	/**
	 * Sends a debuff buildup event to TargetActor's ASC.
	 * SourceActor is the instigator (used to read duration attribute on threshold break).
	 * BuildupAmount is the raw accumulation value — scaled by the passive's immunity ramp if active.
	 */
	UFUNCTION(BlueprintCallable, Category="FP_AbilitySystemLibrary|Debuffs")
	static void ApplyDebuffBuildup(AActor* TargetActor, AActor* SourceActor,
	                               const FGameplayTag& BuildupTag, float BuildupAmount);

	/**
	 * Iterates over a 2D rectangular region of a flat array starting at Index,
	 * spanning Range2D.X columns and Range2D.Y rows within a grid of GridColumns width.
	 * Calls Function on each valid element.
	 */
	template<typename T, typename FuncT>
	static void ForEach2D(TArray<T>& Array, int32 Index, const FIntPoint& Range2D, int32 GridColumns, const FuncT& Function);
};

template<typename T, typename FuncT>
void UFP_AbilitySystemLibrary::ForEach2D(TArray<T>& Array, int32 Index, const FIntPoint& Range2D, int32 GridColumns, const FuncT& Function)
{
	for (int32 j = 0; j < Range2D.Y; ++j)
	{
		for (int32 i = 0; i < Range2D.X; ++i)
		{
			const FIntPoint Coordinates = UFP_WidgetUtils::GetPositionFromIndex(Index, GridColumns) + FIntPoint(i, j);
			const int32 TileIndex = UFP_WidgetUtils::GetIndexFromPosition(Coordinates, GridColumns);
			if (Array.IsValidIndex(TileIndex))
			{
				Function(Array[TileIndex]);
			}
		}
	}
}
