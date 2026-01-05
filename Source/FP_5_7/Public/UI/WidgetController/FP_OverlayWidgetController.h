// Copyright JG

#pragma once

#include "CoreMinimal.h"
#include "FP_WidgetController.h"
#include "FP_OverlayWidgetController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHitPointsChangedSignature, float, NewHitPoints);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHitPointsChangedSignature, float, NewMaxHitPoints);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnHeatChangedSignature, float, NewHeat);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMaxHeatThresholdChangedSignature, float, NewMaxHeatThreshold);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnMinHeatThresholdChangedSignature, float, NewMinHeatThreshold);

/**
 * 
 */
UCLASS(BlueprintType, Blueprintable)
class FP_5_7_API UFP_OverlayWidgetController : public UFP_WidgetController
{
	GENERATED_BODY()
	
public:
	virtual void BroadcastInitialValues() override;
	virtual void BindCallbacksToDependancies() override;
	
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnHitPointsChangedSignature OnHitPointsChanged;

	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnMaxHitPointsChangedSignature OnMaxHitPointsChanged;
	
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnHeatChangedSignature OnHeatChanged;
	
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnMaxHeatThresholdChangedSignature OnMaxHeatThresholdChanged;
	
	UPROPERTY(BlueprintAssignable, Category="GAS|Attributes")
	FOnMinHeatThresholdChangedSignature OnMinHeatThresholdChanged;
	
	
protected:
	void HitPointsChanged(const FOnAttributeChangeData& Data) const;
	void MaxHitPointsChanged(const FOnAttributeChangeData& Data) const;
	void HeatChanged(const FOnAttributeChangeData& Data) const;
	void MaxHeatThresholdChanged(const FOnAttributeChangeData& Data) const;
	void MinHeatThresholdChanged(const FOnAttributeChangeData& Data) const;
	
};
