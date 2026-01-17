// Copyright JG


#include "UI/WidgetController/FP_CharacterMenuWidgetController.h"
#include "AbilitySystem/Data/FP_AttributeInfo.h"
#include "FP_GameplayTags.h"
#include "AbilitySystem/FP_AttributeSet.h"

void UFP_CharacterMenuWidgetController::BindCallbacksToDependencies()
{
	
}

void UFP_CharacterMenuWidgetController::BroadcastInitialValues()
{
	UFP_AttributeSet* AS = CastChecked<UFP_AttributeSet>(AttributeSet);
	check(AttributeInfo);
	
	
	for (auto& Pair: AS->TagsToAttributes){
		FAttributeInfo Info = AttributeInfo->FindAttributeInfoforTag(Pair.Key);
		Info.AttributeValue = Pair.Value().GetNumericValue(AS);
		AttributeInfoDelegate.Broadcast(Info);
	}
}
