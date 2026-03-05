// Copyright JG


#include "UI/WidgetController/FP_CharacterMenuWidgetController.h"
#include "AbilitySystem/Data/FP_AttributeInfo.h"
#include "FP_GameplayTags.h"
#include "AbilitySystem/FP_AttributeSet.h"
#include "Player/FP_PlayerState.h"
#include "AbilitySystem/FP_AbilitySystemComponent.h"

void UFP_CharacterMenuWidgetController::BindCallbacksToDependencies()
{
	UFP_AttributeSet* AS = CastChecked<UFP_AttributeSet>(AttributeSet);
	check(AttributeInfo);
	for (auto& Pair : AS->TagsToAttributes)
	{
		AbilitySystemComponent->GetGameplayAttributeValueChangeDelegate(Pair.Value()).AddLambda(
		[this, Pair](const FOnAttributeChangeData& Data)
		{
			BroadcastAttributeInfo(Pair.Key, Pair.Value());
		});
	}

	AFP_PlayerState* FP_PlayerState = CastChecked<AFP_PlayerState>(PlayerState);
	FP_PlayerState->OnAttributePointsChangedDelegate.AddLambda([this](int32 Points)
	{
		AttributePointsChangedDelegate.Broadcast(Points);
	});
}

void UFP_CharacterMenuWidgetController::BroadcastInitialValues()
{
	UFP_AttributeSet* AS = CastChecked<UFP_AttributeSet>(AttributeSet);
	check(AttributeInfo);

	for (auto& Pair : AS->TagsToAttributes)
	{
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}

	const AFP_PlayerState* FP_PlayerState = CastChecked<AFP_PlayerState>(PlayerState);
	AttributePointsChangedDelegate.Broadcast(FP_PlayerState->GetAttributePoints());
}

void UFP_CharacterMenuWidgetController::UpgradeAttribute(const FGameplayTag& AttributeTag)
{
	UFP_AbilitySystemComponent* FP_ASC = CastChecked<UFP_AbilitySystemComponent>(AbilitySystemComponent);
	FP_ASC->UpgradeAttribute(AttributeTag);
}

void UFP_CharacterMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,
	const FGameplayAttribute& Attribute) const
{
	FAttributeInfo Info = AttributeInfo->FindAttributeInfoforTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
