// Copyright JG


#include "Libraries/FP_AbilitySystemLibrary.h"

#include "AbilitySystem/FP_AttributeSet.h"
#include "Game/FP_GameModeBase.h"
#include "UI/WidgetController/FP_WidgetController.h"
#include "UI\WidgetController/FP_InventoryWidgetController.h"
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

void UFP_AbilitySystemLibrary::GiveStartupAbilities(const UObject* WorldContextObject, UAbilitySystemComponent* ASC)
{
	AFP_GameModeBase* AuraGameMode = Cast<AFP_GameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (AuraGameMode == nullptr) return;

	UCharacterClassInfo* CharacterClassInfo = AuraGameMode->CharacterClassInfo;
	for (TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}
}
