// Copyright JG


#include "UI/WidgetController/FP_OverlayWidgetController.h"

#include "AbilitySystem/FP_AttributeSet.h"

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

void UFP_OverlayWidgetController::BindCallbacksToDependancies()
{
	const UFP_AttributeSet* FP_AttributeSet = CastChecked<UFP_AttributeSet>(AttributeSet);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		FP_AttributeSet->GetHitPointsAttribute()).AddUObject(this, &UFP_OverlayWidgetController::HitPointsChanged);

	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		FP_AttributeSet->GetMaxHitPointsAttribute()).AddUObject(this, &UFP_OverlayWidgetController::MaxHitPointsChanged);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		FP_AttributeSet->GetHeatAttribute()).AddUObject(this, &UFP_OverlayWidgetController::HeatChanged);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		FP_AttributeSet->GetMaxHeatThresholdAttribute()).AddUObject(this, &UFP_OverlayWidgetController::MaxHeatThresholdChanged);
	
	AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(
		FP_AttributeSet->GetMinHeatThresholdAttribute()).AddUObject(this, &UFP_OverlayWidgetController::MinHeatThresholdChanged);
}

void UFP_OverlayWidgetController::HitPointsChanged(const FOnAttributeChangeData& Data) const
{
	OnHitPointsChanged.Broadcast(Data.NewValue);
}

void UFP_OverlayWidgetController::MaxHitPointsChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxHitPointsChanged.Broadcast(Data.NewValue);
}

void UFP_OverlayWidgetController::HeatChanged(const FOnAttributeChangeData& Data) const
{
	OnHeatChanged.Broadcast(Data.NewValue);
}

void UFP_OverlayWidgetController::MaxHeatThresholdChanged(const FOnAttributeChangeData& Data) const
{
	OnMaxHeatThresholdChanged.Broadcast(Data.NewValue);
}

void UFP_OverlayWidgetController::MinHeatThresholdChanged(const FOnAttributeChangeData& Data) const
{
	OnMinHeatThresholdChanged.Broadcast(Data.NewValue);
}
