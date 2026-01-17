// Copyright JG


#include "UI/WidgetController/FP_CharacterMenuWidgetController.h"
#include "AbilitySystem/Data/FP_AttributeInfo.h"
#include "FP_GameplayTags.h"
#include "AbilitySystem/FP_AttributeSet.h"

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
		}
	);
	}
}

void UFP_CharacterMenuWidgetController::BroadcastInitialValues()
{
	UFP_AttributeSet* AS = CastChecked<UFP_AttributeSet>(AttributeSet);
	check(AttributeInfo);
		
	for (auto& Pair: AS->TagsToAttributes){
		BroadcastAttributeInfo(Pair.Key, Pair.Value());
	}
}

void UFP_CharacterMenuWidgetController::BroadcastAttributeInfo(const FGameplayTag& AttributeTag,
	const FGameplayAttribute& Attribute) const
{
	FAttributeInfo Info = AttributeInfo->FindAttributeInfoforTag(AttributeTag);
	Info.AttributeValue = Attribute.GetNumericValue(AttributeSet);
	AttributeInfoDelegate.Broadcast(Info);
}
