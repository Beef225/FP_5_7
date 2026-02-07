// Copyright JG


#include "Libraries/FP_AbilitySystemLibrary.h"

#include "FP_AbilityTypes.h"
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
	UCharacterClassInfo* CharacterClassInfo = GetCharacterClassInfo(WorldContextObject);
	for (TSubclassOf<UGameplayAbility> AbilityClass : CharacterClassInfo->CommonAbilities)
	{
		FGameplayAbilitySpec AbilitySpec = FGameplayAbilitySpec(AbilityClass, 1);
		ASC->GiveAbility(AbilitySpec);
	}
}

UCharacterClassInfo* UFP_AbilitySystemLibrary::GetCharacterClassInfo(const UObject* WorldContextObject)
{
	AFP_GameModeBase* FP_GameMode = Cast<AFP_GameModeBase>(UGameplayStatics::GetGameMode(WorldContextObject));
	if (FP_GameMode == nullptr) return nullptr;
	return FP_GameMode->CharacterClassInfo;
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
