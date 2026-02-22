// Copyright JG


#include "UI/WidgetController/FP_OverlayWidgetController.h"
#include "AbilitySystem/FP_AbilitySystemComponent.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "AbilitySystem/Data/FP_LevelUpInfo.h"
#include "Player/FP_PlayerState.h"

void UFP_OverlayWidgetController::BroadcastInitialValues()
{
	Super::BroadcastInitialValues();
	
	const UFP_AttributeSet* FP_AttributeSet = CastChecked<UFP_AttributeSet>(AttributeSet);
	
	OnHitPointsChanged.Broadcast(FP_AttributeSet->GetHitPoints());
	OnMaxHitPointsChanged.Broadcast(FP_AttributeSet->GetMaxHitPoints());
	OnHeatChanged.Broadcast(FP_AttributeSet->GetHeat());
	OnMaxHeatThresholdChanged.Broadcast(FP_AttributeSet->GetMaxHeatThreshold());
	OnMinHeatThresholdChanged.Broadcast(FP_AttributeSet->GetMinHeatThreshold());
}

void UFP_OverlayWidgetController::BindCallbacksToDependencies()
{
	
	AFP_PlayerState* FPPlayerState = CastChecked<AFP_PlayerState>(PlayerState);
	FPPlayerState->OnXPChangedDelegate.AddUObject(this, &UFP_OverlayWidgetController::OnXPChanged);
	
	const UFP_AttributeSet* FP_AttributeSet = CastChecked<UFP_AttributeSet>(AttributeSet);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(FP_AttributeSet->GetHitPointsAttribute()).AddLambda(
	[this](const FOnAttributeChangeData& Data)
				{OnHitPointsChanged.Broadcast(Data.NewValue);});

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(FP_AttributeSet->GetMaxHitPointsAttribute()).AddLambda(
	[this](const FOnAttributeChangeData& Data)
				{OnMaxHitPointsChanged.Broadcast(Data.NewValue);});
		
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(FP_AttributeSet->GetHeatAttribute()).AddLambda(
	[this](const FOnAttributeChangeData& Data)
				{OnHeatChanged.Broadcast(Data.NewValue);});
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(FP_AttributeSet->GetMaxHeatThresholdAttribute()).AddLambda(
	[this](const FOnAttributeChangeData& Data)
				{OnMaxHeatThresholdChanged.Broadcast(Data.NewValue);});
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(FP_AttributeSet->GetMinHeatThresholdAttribute()).AddLambda(
	[this](const FOnAttributeChangeData& Data)
				{OnMinHeatThresholdChanged.Broadcast(Data.NewValue);});
	
	Cast<UFP_AbilitySystemComponent>(AbilitySystemComponent)->EffectAssetTags.AddLambda(
		[this](const FGameplayTagContainer& AssetTags)
		{
			for (const FGameplayTag& Tag : AssetTags)
			{
				// For example, say that Tag = Message.HealthPotion
				// "Message.HealthPotion".MatchesTag("Message") will return True, "Message".MatchesTag("Message.HealthPotion") will return False
				FGameplayTag MessageTag = FGameplayTag::RequestGameplayTag(FName("Message"));
				if (Tag.MatchesTag(MessageTag))
				{
					const FUIWidgetRow* Row = GetDataTableRowByTag<FUIWidgetRow>(MessageWidgetDataTable, Tag);
					MessageWidgetRowDelegate.Broadcast(*Row);
				}
			}
		}
	);


}

void UFP_OverlayWidgetController::OnXPChanged(int32 NewXP) const
{
	const AFP_PlayerState* AuraPlayerState = CastChecked<AFP_PlayerState>(PlayerState);
	const UFP_LevelUpInfo* LevelUpInfo = AuraPlayerState->LevelUpInfo;
	checkf(LevelUpInfo, TEXT("Unabled to find LevelUpInfo. Please fill out AuraPlayerState Blueprint"));

	const int32 Level = LevelUpInfo->FindLevelForXP(NewXP);
	const int32 MaxLevel = LevelUpInfo->LevelUpInformation.Num();

	if (Level <= MaxLevel && Level > 0)
	{
		const int32 LevelUpRequirement = LevelUpInfo->LevelUpInformation[Level].LevelUpRequirement;
		const int32 PreviousLevelUpRequirement = LevelUpInfo->LevelUpInformation[Level - 1].LevelUpRequirement;

		const int32 DeltaLevelRequirement = LevelUpRequirement - PreviousLevelUpRequirement;
		const int32 XPForThisLevel = NewXP - PreviousLevelUpRequirement;

		const float XPBarPercent = static_cast<float>(XPForThisLevel) / static_cast<float>(DeltaLevelRequirement);

		OnXPPercentChangedDelegate.Broadcast(XPBarPercent);
	}
}
