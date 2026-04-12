// Copyright JG


#include "Libraries/FP_AbilitySystemLibrary.h"

#include "FP_AbilityTypes.h"
#include "Inventory/InventoryManagement/Components/FP_InventoryComponent.h"
#include "UI/Widget/Inventory/FP_InventoryBase.h"
#include "UI/Widget/Inventory/HoverItem/FP_HoverItem.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "Game/FP_GameModeBase.h"
#include "Interaction/FP_CombatInterface.h"
#include "UI/WidgetController/FP_WidgetController.h"
#include "UI\WidgetController/FP_InventoryWidgetController.h"
#include "Engine/OverlapResult.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FP_PlayerState.h"
#include "UI/HUD/FP_HUD.h"

DEFINE_LOG_CATEGORY_STATIC(LogFPAbilitySystemLibrary, Log, All);

UFP_OverlayWidgetController* UFP_AbilitySystemLibrary::GetOverlayWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AFP_HUD* FP_HUD = Cast<AFP_HUD>(PC->GetHUD()))
		{
			AFP_PlayerState* PS = PC->GetPlayerState<AFP_PlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return FP_HUD->GetOverlayWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;

}

UFP_CharacterMenuWidgetController* UFP_AbilitySystemLibrary::GetCharacterWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AFP_HUD* FP_HUD = Cast<AFP_HUD>(PC->GetHUD()))
		{
			AFP_PlayerState* PS = PC->GetPlayerState<AFP_PlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return FP_HUD->GetCharacterMenuWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

UFP_InventoryWidgetController* UFP_AbilitySystemLibrary::GetInventoryWidgetController(const UObject* WorldContextObject)
{
	if (APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (AFP_HUD* FP_HUD = Cast<AFP_HUD>(PC->GetHUD()))
		{
			AFP_PlayerState* PS = PC->GetPlayerState<AFP_PlayerState>();
			UAbilitySystemComponent* ASC = PS->GetAbilitySystemComponent();
			UAttributeSet* AS = PS->GetAttributeSet();
			const FWidgetControllerParams WidgetControllerParams(PC, PS, ASC, AS);
			return FP_HUD->GetInventoryWidgetController(WidgetControllerParams);
		}
	}
	return nullptr;
}

void UFP_AbilitySystemLibrary::InitializeDefaultAttributes(const UObject* WorldContextObject,
	ECharacterClass CharacterClass, float Level, UAbilitySystemComponent* ASC)
{
	AFP_GameModeBase* AuraGameMode = Cast<AFP_GameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr) return;

	AActor* AvatarActor = ASC->GetAvatarActor();

	UCharacterClassInfo* CharacterClassInfo = AuraGameMode->CharacterClassInfo;
	FCharacterClassDefaultInfo ClassDefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);

	FGameplayEffectContextHandle PrimaryAttributesContextHandle = ASC->MakeEffectContext();
	PrimaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryAttributesSpecHandle = ASC->MakeOutgoingSpec(ClassDefaultInfo.PrimaryAttributes, Level, PrimaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryAttributesSpecHandle.Data.Get());

	FGameplayEffectContextHandle SecondaryAttributesContextHandle = ASC->MakeEffectContext();
	SecondaryAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle SecondaryAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->SecondaryAttributes, Level, SecondaryAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*SecondaryAttributesSpecHandle.Data.Get());
	
	FGameplayEffectContextHandle PrimaryDerivedAttributeBonusesContextHandle = ASC->MakeEffectContext();
	PrimaryDerivedAttributeBonusesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle PrimaryDerivedAttributeBonusesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->PrimaryDerivedAttributeBonuses, Level, PrimaryDerivedAttributeBonusesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*PrimaryDerivedAttributeBonusesSpecHandle.Data.Get());


	FGameplayEffectContextHandle VitalAttributesContextHandle = ASC->MakeEffectContext();
	VitalAttributesContextHandle.AddSourceObject(AvatarActor);
	const FGameplayEffectSpecHandle VitalAttributesSpecHandle = ASC->MakeOutgoingSpec(CharacterClassInfo->VitalAttributes, Level, VitalAttributesContextHandle);
	ASC->ApplyGameplayEffectSpecToSelf(*VitalAttributesSpecHandle.Data.Get());
		
}

void UFP_AbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC, ECharacterClass CharacterClass)
{
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (CharacterClassInfo == nullptr) return;
	for (TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}
	
	const FCharacterClassDefaultInfo& DefaultInfo = CharacterClassInfo->GetClassDefaultInfo(CharacterClass);
	for (TSubclassOf<UGameplayAbility> AbilityClass : DefaultInfo.StartupAbilities)
	{
		if (IFP_CombatInterface* CombatInterface = Cast<IFP_CombatInterface>(ASC->GetAvatarActor()))
		{
			FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, CombatInterface->GetPlayerLevel());
			ASC->GiveAbility(AbilitySpec);
		}
	}
}

UCharacterClassInfo* UFP_AbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	AFP_GameModeBase* FP_GameMode = Cast<AFP_GameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (FP_GameMode == nullptr) return nullptr;
	return FP_GameMode->CharacterClassInfo;
}

UFP_SkillLibrary* UFP_AbilitySystemLibrary::GetSkillLibrary(const UObject* WorldContextObject)
{
	if (const APlayerController* PC = UGameplayStatics::GetPlayerController(WorldContextObject, 0))
	{
		if (const AFP_PlayerState* PS = PC->GetPlayerState<AFP_PlayerState>())
		{
			return PS->GetSkillLibrary();
		}
	}

	return nullptr;
}


bool UFP_AbilitySystemLibrary::IsBlockedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FFP_GameplayEffectContext* FP_EffectContext = static_cast<const FFP_GameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return FP_EffectContext->IsBlockedHit();
	}
	return false;
}

bool UFP_AbilitySystemLibrary::IsCriticalHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FFP_GameplayEffectContext* FP_EffectContext = static_cast<const FFP_GameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return FP_EffectContext->IsCriticalHit();
	}
	return false;
}

bool UFP_AbilitySystemLibrary::IsDodgedHit(const FGameplayEffectContextHandle& EffectContextHandle)
{
	if (const FFP_GameplayEffectContext* FP_EffectContext = static_cast<const FFP_GameplayEffectContext*>(EffectContextHandle.Get()))
	{
		return FP_EffectContext->IsDodgedHit();
	}
	return false;
}

void UFP_AbilitySystemLibrary::SetIsBlockedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsBlockedHit)
{
	if ( FFP_GameplayEffectContext* FP_EffectContext = static_cast<FFP_GameplayEffectContext*>(EffectContextHandle.Get()))
	{
		FP_EffectContext->SetIsBlockedHit(bInIsBlockedHit);
	}
}

void UFP_AbilitySystemLibrary::SetIsCriticalHit(FGameplayEffectContextHandle& EffectContextHandle,
	bool bInIsCriticalHit)
{
	if ( FFP_GameplayEffectContext* FP_EffectContext = static_cast<FFP_GameplayEffectContext*>(EffectContextHandle.Get()))
	{
		FP_EffectContext->SetIsCriticalHit(bInIsCriticalHit);
	}
}

void UFP_AbilitySystemLibrary::SetIsDodgedHit(FGameplayEffectContextHandle& EffectContextHandle, bool bInIsDodgedHit)
{
	if ( FFP_GameplayEffectContext* FP_EffectContext = static_cast<FFP_GameplayEffectContext*>(EffectContextHandle.Get()))
	{
		FP_EffectContext->SetIsDodgedHit(bInIsDodgedHit);
	}
}

void UFP_AbilitySystemLibrary::GetLivePlayersWithinRadius(const UObject* WorldContextObject,
	TArray<AActor*>& OutOverlappingActors, const TArray<AActor*>& ActorsToIgnore, float Radius,
	const FVector& SphereOrigin)
{
	FCollisionQueryParams SphereParams;
	SphereParams.AddIgnoredActors(ActorsToIgnore);
	
	if (const UWorld* World = GEngine->GetWorldFromContextObject(WorldContextObject, EGetWorldErrorMode::LogAndReturnNull))
	{
		TArray<FOverlapResult> Overlaps;
		World->OverlapMultiByObjectType(Overlaps, SphereOrigin, FQuat::Identity, FCollisionObjectQueryParams(FCollisionObjectQueryParams::InitType::AllDynamicObjects), FCollisionShape::MakeSphere(Radius), SphereParams);
		for (FOverlapResult& Overlap : Overlaps)
		{
			if (Overlap.GetActor()->Implements<UFP_CombatInterface>() && !IFP_CombatInterface::Execute_IsDead(Overlap.GetActor()))
			{
				OutOverlappingActors.AddUnique(IFP_CombatInterface::Execute_GetAvatar(Overlap.GetActor()));
			}
		}
	}
}

bool UFP_AbilitySystemLibrary::IsNotFriend(AActor* FirstActor, AActor* SecondActor)
{
	const bool bBothArePlayers = FirstActor->ActorHasTag(FName("Player")) && SecondActor->ActorHasTag(FName("Player"));
	const bool bBothAreEnemies = FirstActor->ActorHasTag(FName("Enemy")) && SecondActor->ActorHasTag(FName("Enemy"));
	const bool bFriends = bBothArePlayers || bBothAreEnemies;
	return !bFriends;
}

int32 UFP_AbilitySystemLibrary::GetXPReward(const UObject* WorldContextObject, int32 BaseXP, int32 CharacterLevel)
{
	const UCharacterClassInfo* ClassInfo = GetCharacterClassInfo(WorldContextObject);
	if (!ClassInfo) return BaseXP;

	const float Coefficient = ClassInfo->XPLevelCoefficient;
	const float LevelScale = 1.f + Coefficient * FMath::Square(static_cast<float>(CharacterLevel - 1));
	return FMath::RoundToInt(static_cast<float>(BaseXP) * LevelScale);
}

UFP_LootTiers* UFP_AbilitySystemLibrary::GetLootTiers(const UObject* WorldContextObject)
{
	const AFP_GameModeBase* GameMode = Cast<AFP_GameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (!GameMode) return nullptr;
	return GameMode->LootTiers;
}

TArray<FLootItem> UFP_AbilitySystemLibrary::GetLootItemsForEnemy(const UObject* WorldContextObject, const TArray<UFP_LootTiers*>& ExtraLootTiers)
{
	TArray<FLootItem> Result;

	// Roll global pool (every enemy)
	if (UFP_LootTiers* GlobalTiers = GetLootTiers(WorldContextObject))
	{
		Result.Append(GlobalTiers->GetLootItems());
	}

	// Roll any extra tables assigned to this specific enemy
	for (UFP_LootTiers* Extra : ExtraLootTiers)
	{
		if (Extra)
		{
			Result.Append(Extra->GetLootItems());
		}
	}

	return Result;
}

UFP_InventoryComponent* UFP_AbilitySystemLibrary::GetInventoryComponent(const APlayerController* PlayerController)
{
	if (!IsValid(PlayerController)) return nullptr;
	return PlayerController->FindComponentByClass<UFP_InventoryComponent>();
}

EItemCategory UFP_AbilitySystemLibrary::GetItemCategoryFromItemComp(UFP_ItemComponent* ItemComp)
{
	if (!IsValid(ItemComp)) return EItemCategory::None;
	return ItemComp->GetItemManifest().GetItemCategory();
}

UFP_HoverItem* UFP_AbilitySystemLibrary::GetHoverItem(APlayerController* PC)
{
	UFP_InventoryComponent* IC = GetInventoryComponent(PC);
	if (!IsValid(IC)) return nullptr;
	UFP_InventoryBase* InventoryMenu = IC->GetInventoryMenu();
	if (!IsValid(InventoryMenu)) return nullptr;
	return InventoryMenu->GetHoverItem();
}
