// Copyright JG


#include "Libraries/FP_AbilitySystemLibrary.h"
#include "UI/WidgetController/FP_WidgetController.h"
#include "UI\WidgetController/FP_InventoryWidgetController.h"
#include "Kismet/GameplayStatics.h"
#include "Player/FP_PlayerState.h"
#include "UI/HUD/FP_HUD.h"

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
